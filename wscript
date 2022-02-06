top = '.'
out = 'bin'

def configure(conf):
    pass

def build(bld):
    c_files = bld.path.ant_glob("src/*.c")
    sdl_flags = "-s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=\'[\"png\"]\'"
    preload_assets = "--preload-file res"
    bld(rule = 'emcc ${SRC} %s %s -O2 -o index.html'%(sdl_flags, preload_assets), source = c_files)
