examples = ['keystate', 'outputs', 'screenshot', 'toplevels']

foreach example : examples
    executable(
        example,

        sources: [example + '.c'],

        dependencies: [
            xw,
            dependency('gtk+-3.0')
        ]
    )
endforeach
