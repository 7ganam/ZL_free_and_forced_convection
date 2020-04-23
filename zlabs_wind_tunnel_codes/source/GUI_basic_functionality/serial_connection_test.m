delete(instrfindall);
com_name = find_arduino_com_windowsOS()
connection = serial(com_name,'BaudRate',115200)
fopen(connection)