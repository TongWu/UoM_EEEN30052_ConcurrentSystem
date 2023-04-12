#include <iostream>
#include <random>
#include <ctime>

using namespace std;

std::mt19937 gen(time(0)); //Standard Mersenne_twister_engine seeded with time(0)

int sleepRandomPeriod()  {
  std::uniform_int_distribution<> dis(1, 10); //generate a random integer between 1-10.
  int n = dis(gen);
  return n;
}
int main()
{
    cout << "Hello world!" << endl;
    for (int i = 0; i < 20; i++)
        cout << sleepRandomPeriod() << endl;
    return 0;
}