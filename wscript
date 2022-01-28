top = '.'
out = 'bin'

from waflib.Task import Task

def configure(conf):
    pass

def build(bld):
    c_files = bld.path.ant_glob("src/*.c")
    bld(rule = 'emcc ${SRC} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=\'[\"png\"]\' -o index.html', source = c_files)
