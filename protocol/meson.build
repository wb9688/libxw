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
    'keystate.xml'
]

client_protos_src = []
client_protos_headers = []

foreach p : client_protocols
    client_protos_src += wayland_scanner_code.process(p)
    client_protos_headers += wayland_scanner_client.process(p)
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
