function explode_string(str)
   char_array = {}
   for i = 1,string.len(str) do
      char_array[i] = string.sub(str, i, i)
   end

   return char_array
end

function string_num_sum(term_a, term_b)
   term_a = explode_string(term_a)
   term_b = explode_string(term_b)

   -- Put the lesser character array in term_a
   if table.getn(term_a) > table.getn(term_b) then
      tmp = term_a
      term_a = term_b
      term_b = tmp
   end

   -- Equals the size of arrays term_a and term_b filling
   -- the new itens in term_a with '0'
   for i = 1, (table.getn(term_b) - table.getn(term_a)) do
      table.insert(term_a, 1, '0')
   end
   size = table.getn(term_a)

   carry = 0
   sum = {}
   for i= size,1,-1 do

      sum_part = term_a[i] + term_b[i] + carry

      carry = math.floor(sum_part/10)
      sum_part = math.mod(sum_part, 10)

      sum[i] = tostring(sum_part)
   end
   if carry ~= 0 then
      table.insert(sum, 1, tostring(carry))
   end

   answer = ''
   for i = 1,table.getn(sum) do
      answer = answer .. sum[i]
   end

   return answer
end

function dragonfly2uid(dragonfly_id)
   dragonfly_id = tostring(dragonfly_id)
   return string_num_sum('49712223521603584', dragonfly_id)
end
