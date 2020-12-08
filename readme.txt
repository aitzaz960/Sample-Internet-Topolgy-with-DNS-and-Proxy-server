--> For compiling enter following commands
	1. g++ DNS.cpp -o dns
	2. g++ proxy.cpp -o proxy
	3. g++ google.cpp -o google
	4. g++ flex.cpp -o flex
	5. g++ nu.cpp -o nu
	6. g++ slate.cpp -o slate
	7. g++ client.cpp -o client
		
-->You will have to open ./dns and ./proxy in seperate tabs of terminal each time to run the program otherwise client won't be able to connect with servers.
-->Open as many client as you want by entering ./client in seperate terminals but as there are 4 servers so you can only open 4 servers at a time at max and dont open two servers with same name
-->When you will be asked to enter the url please enter any of the correct urls provided in IPs.txt which are following
	google.com
	flexstudent.nu.edu.pk
	nu.edu.pk
	slate.nu.edu.pk
-->The proxy server is implemented in a way that if enetered url is same as the most recent url, the response will come from proxy server and not the dns
-->My implemented topology is different than the topology in pdf in two ways
	1.) In my topology every server shares its routing table with every other server and hence its connected to every other server
	2.) Every client is directly connected with proxy which in turn with DNS unlike the topology of pdf
