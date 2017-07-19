module Location
  def self.where_am_i
    puts "haha"
  end

  def show_location
    puts "#{self.class} show location"
  end
end


class Person
  attr_accessor :first_name, :last_name
  def greet
    puts "Hellow, #{@first_name} #{@last_name}"
  end
end

p1 = Person.new
p1.first_name = "Peter"
p1.last_name = "Wang"
p1.greet # 输出 "Hello, Peter Wang"

p2 = Person.new
p2.first_name = "William"
p2.last_name = "Zhang"
p2.greet # 输出 "Hello, William Zhang"

class Man < Person
  include Location
end

m = Man.new
m.show_location()

Location.where_am_i()
