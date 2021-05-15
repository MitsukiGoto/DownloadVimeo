#include <iostream>
#include <thread>

int main()
{
    for (int i = 0; i <= 20; i++)
    {
        std::cout << "\r"
                  << "[" << std::string(i, '.') << std::string(20 - i, ' ') << "]";
                  std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}