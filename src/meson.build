cc = meson.get_compiler('c')

libxw = shared_library(
    'xw',

    sources: [
        'keystate.c',
        'keystate-kde.c',
        'keystate-x11.c',
        'output.c',
        'output-wlroots.c',
        'outputs.c',
        'outputs-wlroots.c',
        'screenshot.c',
        'screenshot-wlroots.c'
    ],

    dependencies: [
        cc.find_library('rt'),
        dependency('glib-2.0'),
        dependency('gdk-pixbuf-2.0'),
        dependency('gdk-x11-3.0'),
        dependency('gdk-wayland-3.0'),
        dependency('x11'),
        client_protos,
        wayland_client
    ],

    include_directories: include_directories('../include'),

    install: true
)

pkgconfig = import('pkgconfig')
pkgconfig.generate(libxw, subdirs: ['libxw'])

xw = declare_dependency(
    link_with: libxw,
    include_directories: include_directories('../include')
)
