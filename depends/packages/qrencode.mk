package=qrencode
$(package)_version=3.4.4
$(package)_download_path=https://github.com/fukuchi/libqrencode/archive/refs/tags/
$(package)_file_name=v3.4.4.tar.gz
$(package)_sha256_hash=ab7cdf84e3707573a39e116ebd33faa513b306201941df3f5e691568368d87bf
$(package)_extract_dir=libqrencode-3.4.4

define $(package)_preprocess_cmds
  mkdir -p m4 && ./autogen.sh
endef

define $(package)_set_vars
  $(package)_config_opts=--disable-shared --without-tools --disable-sdltest
  $(package)_config_opts_linux=--with-pic
endef

define $(package)_config_cmds
  ./configure --host=$($(package)_host) --prefix=$($(package)_staging_prefix_dir) $($(package)_config_opts)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
