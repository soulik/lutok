require 'example_object'

local example1 = ExampleObject()
local example2 = ExampleObject()
			
example1.doSomething()
example2.doSomething2()
example1.x = 5
print(example1.x)

-- class ExampleObject should have got overriden operator +
-- local example3 = example1 + example2
