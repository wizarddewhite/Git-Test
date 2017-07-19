# 请打开 wordcount.txt，计算每个单字出现的次数

doc = File.read("wordcount.txt")

# ...
h = Hash.new

doc.each_line do |line|
  # puts line
  words = line.split

  words.each do |w|
    if h.has_key?(w)
      h[w] += 1
    else
      h[w] = 1
    end
  end
end

h.sort.each do |word, count|
  puts " #{word} appears #{count.to_s} times"
end
