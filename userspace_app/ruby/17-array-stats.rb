# 使用者不断输入数字存进 Array，最后输出总和、平均、最大值、最小值

arr = []
max = 0
min = 0
total = 0

while (true)
  print "请输入数字，结束请直接按 Enter: "
  user_input = gets
  if user_input == "\n"
    break
  else
    arr << user_input.to_i
  end

  total += user_input.to_i
  max = (user_input.to_i > max) ? user_input.to_i : max
  min = (user_input.to_i < min) ? user_input.to_i : min
end

puts arr.to_s

avg = total / arr.size

puts "总和是 #{total.to_s}"
puts "平均是 #{avg.to_s}"
puts "最大值是 #{max.to_s}"
puts "最小值是 #{min.to_s}"
