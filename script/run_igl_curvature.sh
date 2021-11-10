# for i in 0 1 2 3 5 6
# do
# 	path=../data/RigNet/13/${i}.off
# 	echo ${path}
# 	../build/igl_curvature ${path} ../output/RigNet/13/${i}.h ../output/RigNet/13/${i}.k
# 	echo
# done

for i in 0 1 2 3 5 7 8 9
do
	path=../data/RigNet/7/${i}.off
	echo ${path}
	../build/igl_curvature ${path} ../output/RigNet/7/${i}.h ../output/RigNet/7/${i}.k
	echo
done