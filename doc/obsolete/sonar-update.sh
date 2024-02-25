cppcheck --xml --xml-version=2 --enable=all /home/borsanza/c/awui/libawui 2> cppcheck-report.xml

../../sonar-scanner/bin/sonar-scanner \
	-Dsonar.projectKey=awui \
	-Dsonar.sources=. \
	-Dsonar.host.url=http://twenty:9000 \
	-Dsonar.login=8774a3252add0b0d31b8fefcc265e61910ab93c1
