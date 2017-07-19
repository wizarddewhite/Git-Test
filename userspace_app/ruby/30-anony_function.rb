foo = -> { puts "foo" }

def bar(x)
  puts "bar"
  x.call
end

bar(foo)

# inline way
bar( -> { puts "foo" } )

# the do way
def bar1
  puts "bar"
  yield
end

bar1 { puts "foo" }

bar1 do
  puts "foo"
end

# mixture
puts "mixture callback"
def bar2(&block)
  puts "bar"
  block.call("x")
end

bar2 do |x|
  puts x
end

class Person
  def for_each_person
    for i in 0..5
      yield(i)
    end
  end
end

P = Person.new

P.for_each_person do |p|
  puts p
end

# the same as above
# P.for_each_person { |p| puts p}

arr = [1,2,3,4,5,6,7,8,9,10]

result = arr.map do |i|
  i + 1
end
# the same as above
#result = arr.map { |i| i+1 }

puts result
