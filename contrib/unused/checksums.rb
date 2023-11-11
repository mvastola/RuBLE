# frozen_string_literal: true

module RubBLE
  module Build
    class Checksums
      CHECKSUMS_FILE_RELATIVE = 'CHECKSUMS'
      DIGEST_ALGO = 'SHA512'
      READ_BUFFER_SIZE = 8192

      class << self
        def hash_file(path, digest = DIGEST_ALGO)
          hasher = OpenSSL::Digest.new(digest.to_s)
          File.open(path.to_s, 'rb') do |f|
            hasher << f.readpartial(READ_BUFFER_SIZE) until f.eof?
          end
          hasher.hexdigest
        rescue RefError # TODO: catch file not found errors (this is placeholder)
          nil
        end

        def hash_file_promise(path, *args, directory:, executor: :io)
          Concurrent::Promise.new(executor:) do
            [
              path_to_relative(path, directory:),
              hash_file(path_to_absolute(path, directory:), *args),
            ]
          end
        end

        def hash_files(*files, directory:)
          # thread_pool = Concurrent::FixedThreadPool.new(Etc.nprocessors)
          promises = files.map do |file|
            hash_file_promise(file, directory:) #, executor: thread_pool)
          end

          Concurrent::Promise.zip(*promises, executor: :fast).then do |results|
            hash = results.to_h
            paths_given = files.to_set
            paths_returned = hash.keys.to_set
            unless paths_given == paths_returned
              extra_files = paths_returned - paths_given
              missing_files = paths_given - paths_returned
              raise 'File list mismatch when generating checksums! '\
                    "Extra Files:\n#{extra_files.to_a.join("\n")}\n"\
                    "Missing Files:\n#{missing_files.to_a.join("\n")}"
            end
            hash
          end
        ensure
          # thread_pool&.shutdown
          # thread_pool&.wait_for_termination
        end

        def path_to_relative(path, directory:)
          path = Pathname.new(path)
          path = path.relative_path_from(directory) if path.absolute?
          path.cleanpath(false)
        end

        def path_to_absolute(path, directory:)
          path = Pathname.new(path)
          path = path.absolute? ? path : (directory / path)
          path.cleanpath(false)
        end
      end

      attr_reader :directory
      def initialize(directory)
        @directory = Pathname.new(directory).cleanpath(false).freeze
      end

      def checksums_file = directory / CHECKSUMS_FILE_RELATIVE

      def checksummable_files
        directory.find.lazy.select do |path|
          path.file? && !path.symlink? && path != checksums_file
        end.map(&method(:path_to_relative))
      end

      def checksum_promise = self.class.hash_files(*checksummable_files, directory:)

      def write_checksums
        checksums = checksum_promise.tap(&:wait!).value!
        checksums_file.write(JSON.pretty_generate(checksums))
      end

      def verify_checksums
        return false unless checksums_file.exist?

        current_checksums = checksum_promise.tap(&:wait!).value!
        raise "No files found in #{directory}" if current_checksums.empty?

        max_mtime = current_checksums.keys.map { (directory / _1).mtime }.max
        if checksums_file.mtime < max_mtime
          warn "Checksum file (#{checksums_file}) is older than newest file. Ignoring it. "\
               'Checksum verification failed.'
          return false
        end

        stored_checksums = JSON.parse(checksums_file.read)
                               .transform_keys!(&method(:path_to_relative))

        unless current_checksums.keys.to_set == stored_checksums.keys.to_set
          warn "Files have been added and/or removed from #{directory} since checksums "\
               'were generated. Checksum verification failed.'
          return false
        end

        unless stored_checksums == current_checksums
          warn "Checksum mismatch. Expected:\n#{stored_checksums}\nGot:\n#{current_checksums}"
          return false
        end
        true
      end

      def path_to_relative(path)
        self.class.path_to_relative(path, directory:)
      end

      def path_to_absolute(path)
        self.class.path_to_absolute(path, directory:)
      end
    end
  end
end
