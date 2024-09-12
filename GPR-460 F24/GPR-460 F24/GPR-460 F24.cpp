#include <iostream>
#include <unordered_map>
#include <vector>

/******* Test 1 *************/
void PrintHello() { std::cout << "Hello!\n"; }

void PrintGoodbye() { std::cout << "Goodbye!\n"; }

template <typename T> void TestType() {
    std::vector<T> fns;
    fns.push_back(PrintHello);
    fns.push_back(PrintGoodbye);

    for (T fn : fns) {
        fn();
    }
}
/********************************/

/*********** TEST 2 *************/
bool quitMenu = false;
typedef int (*MenuFn)(int);
typedef std::unordered_map<char, MenuFn> MenuMap;
const char testInput[] = { 'a', 's', 'a', 'm', 's', 'm', 'q', '\0' };

char getNextChar(bool useTestInput = true)
{
    char input = '\0';
    if (useTestInput)
    {
        static const char* nextChar = testInput;
        input = *nextChar;
        if (input != '\0')
        {
            nextChar++;
        }
    }
    else
    {
        std::cin >> input;
        std::cin.ignore(256, '\n');
    }

    return input;
}

int TestMenu(MenuMap menuOptions, bool useTestInput = true) {
    int value = 0;
    while (!quitMenu) {
        std::cout << "Current value: " << value << std::endl;
        std::cout << "Enter an option:\n\n";
        std::cout << "a. Add 5 to current value\n";
        std::cout << "s. Subtract 3 from current value\n";
        std::cout << "m. Multiply current value by 2\n";
        std::cout << "q. Quit\n\n";

        char input = getNextChar(useTestInput);

        if (menuOptions.find(input) == menuOptions.end()) {
            std::cout << "Option '" << input << "' has no function associated with it.\n\n";
            continue;
        }

        value = menuOptions[input](value);
    }

    std::cout << "Exiting.\n";
    return value;
}

// TODO: Write functions to be held in menuOptions here
int AddMenu(int number) {
    return number + 5;
}

int SubtractMenu(int number) {
    return number - 3;
}

int MultiplyMenu(int number) {
    return number * 2;
}

// ----------

int QuitMenu(int number) {
    quitMenu = true;

    // Don't modify the value.
    return number;
}

/***********************************/

int main() {
    std::cout << "Test 1: You should see \"Hello!\" and \"Goodbye!\" on separate "
        "lines:\n";
    // TODO: Uncomment the next line, substituting in the correct type.
    TestType <void(*)()>();
    std::cout << "----------------------\n\n";

    MenuMap menuOptions;
    // TODO: Bind menu items for characters 'm', 'a', and 's'
    //       to their respective functions.
    menuOptions['a'] = AddMenu;
    menuOptions['s'] = SubtractMenu;
    menuOptions['m'] = MultiplyMenu;
    menuOptions['q'] = QuitMenu;

    std::cout << "Test 2\n";
    int result = TestMenu(menuOptions);
    int expected = 22;
    std::cout << "Test 2 " << (result == expected ? "PASSED" : "FAILED")
        << ": Got " << result << (result == expected ? " and" : " but")
        << " expected " << expected << std::endl;
    std::cout << "------------------------\n\n";
}