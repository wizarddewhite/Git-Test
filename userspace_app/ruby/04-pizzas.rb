# 题目: 输入有多少片比萨饼和多少人，输出每人可以分到几片，以及剩下几片

print "请输入有多少片比萨饼，然后按 Enter: "
pizzas = gets

print "请输入有多少人要吃，然后按 Enter: "
people = gets

pizza_per_one = pizzas.to_i / people.to_i
left = pizzas.to_i - pizza_per_one * people.to_i

puts "每人可分得几片: #{pizza_per_one.to_s} 片"
puts "还剩下几片: #{left.to_s} 片"