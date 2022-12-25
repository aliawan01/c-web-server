/*
   pseudo code for http server:

   int main(void) {

		// socket and bind
		int server = http_create_server("localhost", 8080);

		// setting up routes
		http_send_js();
		http_handle_route("GET", "/main_page", char* function_pointer());
		http_handle_route("POST", "/main_page", char* function_pointer(dict* data_received));
											    



		// Run the server
		http_run_server(server);



	   return 0;
   }



*/
