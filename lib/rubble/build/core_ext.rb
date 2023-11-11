# frozen_string_literal: true

module Kernel
  def is_any_of?(*klasses) = klasses.any? { is_a?(_1) }
end