libxw = shared_library(
    'xw',

    sources: [
        'keystate.c',
        'keystate-kde.c',
        'keystate-x11.c'
    ],

    dependencies: [
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

xw = declare_dependency(
    link_with: libxw,
    include_directories: include_directories('../include')
)
