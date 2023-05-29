#include <iostream>
#include <thread>
#include <unordered_map>
/* if on Linux: */
#include <unistd.h>
//#include <windows.h>
#include <mutex>

bool A = false;
bool B = false;
bool C = false;

std::mutex mu;
char stopping_state = '.';

void starting() {
   if (A) {
      if (B) {
         C = true;
         return;
      }
      else {
         B = true;
         sleep(2);
      }
   }
   else {
      A = true;
      sleep(2);
   }
   starting();
}

void stopping(bool& state) {
   state = false;
   if (&A == &state) {
      sleep(2);
      stopping(B);
   }
   else if (&B == &state) {
      sleep(2);
      stopping(C);
   } 
   else if (&C == &state) {
      sleep(2);
      starting();
   }
}


void state_machine() {
   if (!A) {
      B = false;
      C = false;
   } 
   else {
      if (!B) {
         C = false;

      }
   }
}

char get_stopping_state() {
   std::lock_guard<std::mutex> guard(mu);
   return stopping_state;
}

void state_manager() {
   starting();
   while (1) {
      char state = get_stopping_state();
      if (state != '.') {
         switch (state)
         {
         case 'A':
            stopping(A);
            break;
         case 'B':
            stopping(B);
            break;
         case 'C':
            stopping(C);
            break;
         default:
            break;
         }
      }
   }
}

void set_stopping_state(char c) {
   std::lock_guard<std::mutex> guard(mu);
   stopping_state = c;
}

void reading_input() {
   char c;
   while (1) {
      std::cin>>c;
      if (c == 'A' || c == 'B' || c == 'C') {
         set_stopping_state(c);
         std::cout << "Gasenje " << c << std::endl;
      }
      else {
         std::cout << "No such state" << std::endl;
      }
      
   }
}

int main(void) {
   std::thread first (state_manager);
   std::thread reader (reading_input);

   std::unordered_map<bool, std::string> state = {{true, "upaljeno"}, {false, "ugaseno"}};

   while (1) {
      std::cout << "A " << state[A] << std::endl 
             << "B " << state[B] << std::endl 
             << "C " << state[C] << std::endl;

      sleep(5);
   }
   
   return 0;
}

