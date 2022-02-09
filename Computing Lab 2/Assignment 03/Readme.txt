Assignment 03
Computing lab II 
 
Name  Md Ashik Khan
ID    21CS60A02

(on linux kernal)
To compile the server program :  gcc server.c -o server -lm
To run server program :   ./server

To compile client program :  gcc client.c -o client 
To run client program :   ./client

FTP Commands to be given as:

1. /users
2. /files
3. /upload <filename>
4. /download <filename>
5. /invite <filename> <client_id> <permission>
6. /read <filename> <start_idx> <end_idx>
7. /insert <filename> <idx> “<message>”
8. /delete <filename> <start_idx> <end_idx>
9. /exit
10./profile

Possible case considered :

1. /users:  
		# Counting the clients with active status and returning at the client terminal.
2. /files: 
		# returning the files with file owner Id , No of lines and total collaboratos , collaborator list with collaborator permission and returning at the client terminal.
		# No Collaborator, Total Collaborator is 0 and No List.
3. /upload <filename>: 
		# Check the file is valid/exists in the client side .
		# Check the filename is exists in the server side .
		# If exist  upload in the server end.
		# Storing the filename and owner Id in the File Record Data Structure.
4. /download <filename>: 
		# Check the file exists in server .
		# Check the client has permission to download the file ( Owner, Collaboratos with V\E has permission).
		# If exist download in the client end.
5. /invite <filename> <client_id> <permission>: 
		# Check the file exists in server .
		# Check the Client ID is valid.
		# Check the permission name is valid.
		# Check the requested Client is the owner or not.
		# Check the invited Client has the permission already or not.
		# If the CLient has permission already , Server notify this to requested Client.
		# Server forward the Invitation to Invited Client.
		# Invited Client sends back the response to Server.
		# For acceptance,Server stores the invited client as collaborator with the permission for the file.
		# Server replies the update to both requested Client and invited client. 
6. /read <filename> <start_idx> <end_idx>:
		# Check the file exists in server .
		# Check the client has permission to read the file ( Owner, Collaboratos with V\E has permission).
		# Check end_idx is present or not, if not , make end_idx  equal to start_idx.
		# Check start_idx and end_idx is present or not, if not , make start_idx equal to 0 and end_idx  equal to last line.
		# Check start_idx and end_idx is between (-N,N-1).
7. /insert <filename> <idx> “<message>”:
		# Check the file exists in server .
		# Check the client has permission to read the file ( Owner, Collaboratos with E has permission).
		# Check idx is between (-N+1,N).
		# Check idx is present or not, if not , make idx  equal to (last_line+1).
8. /delete <filename> <start_idx> <end_idx>: 
		# Check the file exists in server .
		# Check the client has permission to read the file ( Owner, Collaboratos with E has permission).
		# Check end_idx is present or not, if not , make end_idx  equal to start_idx.
		# Check start_idx and end_idx is present or not, if not , make start_idx equal to 0 and end_idx  equal to total file Lines .
		# Check start_idx and end_idx is between (-N,N-1).
9. /exit: 
		# Client left ,CLient Soket Closed.
		# CLient List at server end updated.
10./profile: 
		# Return Client Unique ID
