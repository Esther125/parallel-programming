echo "t,serial_ms,thread_ms,speedup"
for t in 1 2 3 4 5 6; do
  out=$(run -c $t -- ./mandelbrot -t $t --view 1)
  s=$(echo "$out" | awk '/\[mandelbrot serial\]/{print $3}' | tr -d '[]')
  p=$(echo "$out" | awk '/\[mandelbrot thread\]/{print $3}' | tr -d '[]')
  sp=$(echo "$out" | grep -oP '\([0-9.]+x' | tr -d '()x')
  echo "$t,$s,$p,$sp"
done