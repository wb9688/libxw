wayland_scanner = find_program('wayland-scanner')

wayland_scanner_code = generator(
    wayland_scanner,
    output: '@BASENAME@-protocol.c',
    arguments: ['private-code', '@INPUT@', '@OUTPUT@']
)

wayland_scanner_client = generator(
    wayland_scanner,
    output: '@BASENAME@-client-protocol.h',
    arguments: ['client-header', '@INPUT@', '@OUTPUT@']
)

client_protocols = [
    'keystate.xml',
    'wlr-foreign-toplevel-management-unstable-v1.xml',
    'wlr-output-management-unstable-v1.xml',
    'wlr-screencopy-unstable-v1.xml'
]

client_protos_src = []
client_protos_headers = []

foreach client_protocol : client_protocols
    client_protos_src += wayland_scanner_code.process(client_protocol)
    client_protos_headers += wayland_scanner_client.process(client_protocol)
endforeach

lib_client_protos = static_library(
    'client_protos',

    client_protos_src + client_protos_headers,

    dependencies: [wayland_client]
)

client_protos = declare_dependency(
    link_with: lib_client_protos,
    sources: client_protos_headers
)
