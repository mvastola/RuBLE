# Included from ./Makefile


# TODO: add compile rules for `all` and `install` to
#   create build dir and invoke cmake with appropriate arguments


# install: install-so install-rb
# install-so: Makefile
# install-rb: pre-install-rb do-install-rb install-rb-default
# install-rb-default: pre-install-rb-default do-install-rb-default
# all:    Makefile
# static: $(STATIC_LIB)
# .PHONY: all install static install-so install-rb
# .PHONY: clean clean-so clean-static clean-rb

pre-build: # TODO: add deps
  echo mkdir -p $(build_dir)
  echo cd $(build_dir)
  echo cmake $(ext_dir) -A $(plattorm) -G 'Unix Makefile' -C build-config.cmake --install-prefix ...


$(DLLIB): pre-build
  # should install to $(TARGET_SO_DIR)
  echo make -C $(build_dir) $@
  

# TODO add static target in cmake
$(STATIC_LIB): pre-build
  echo make -C $(build_dir) $@
  echo "ERROR: Static builds not supported"
  exit 2

so: $(DLLIB) # todo: is this already defined?

all: so static

install-static: static
  # todo: why don't we not use cmake at all for the install?
  echo make -C $(build_dir) $@

install-so-local: so
  # copy so into lib/rubble/rubble.so

install-so: so
  # todo: why don't we not use cmake at all for the install?
  echo make -C $(build_dir) $@

clean-static:
  echo make -C $(build_dir) clean

clean-so:
  echo make -C $(build_dir) clean

distclean-so:
  rm -Rf $(build_dir)

distclean-static:
  rm -Rf $(build_dir)



