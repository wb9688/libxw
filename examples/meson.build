examples = ['keystate', 'outputs', 'screenshot']

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
