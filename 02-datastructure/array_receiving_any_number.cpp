#include<array>
#include<iostream>
#include<type_traits>

/*
숫자 타입이기만 하면 전부 입력 받을 수 있는 배열을 리턴하는 함수를 활용한다.
실행 결과는 아래와 같다.

1 0 97 3.2 0
Park Dongvin

*/

// ... Args를 사용함으로써 가변 템플릿임을 알린다.
template<typename ... Args>
// forwarding reference가 사용된 인자다. 함수가 lvaluse 또는 rvalue를 인자로서
// 받을 수 있게 해주고, std::forward를 사용해서 다른 함수 또는 객체에
// 연산 결과를 전송할 수 있게 해준다.
auto build_array(Args&&... args)
// 리턴 타입이 함수 인자에 의존적이기 때문에, 후행 리턴 타입(trailing return type)으로 돼 있다.
// 후행 리턴 타입이기 때문에, 인자를 먼저 받아들이고 나서 함수 자신의 리턴 타입을 결졍하는 것이 허용된다.
// 복잡해 보이지만, std::array<배열의 타입, 배열의 사이즈> 라는 구조다.
// std::common_type<Args...>::type 이 부분이 입력으로 들어온 인자들을 전부
// 포괄할 수 있는 타입으로 바꿔주는 역할을 한다.
// 아래의 main()의 경우, double 이다.
    -> std::array<typename std::common_type<Args...>::type, sizeof...(args)>
{
    // 타입 이름이 길어서 짧은 말을 정의해 준다.
    using commonType = typename std::common_type<Args...>::type;

    // return  {/*...*/}; 는 Aggregate Initialinzation으로 객체를 만들어서 
    // 객체를 리턴시킨다는 의미다.
    // using commonType 으로 정의한, 배열 원소 타입 정보를 바탕으로,
    // 인자로 들어온 각각의 값들에 대해서 type deduction과, std::forward 연산이 적용된다.
    // std::forward 를 사용함으로써, 함수 내부에서 만들어진 array 객체가
    // 함수 바깥으로 이동 될 때 불필요한 copy가 일어나지 않게 해준다.
    return {std::forward<commonType>((Args&&)args)...};
}

int main(){

    // int, unsigned int, char, float, bool 타입이다.
    auto data = build_array(1, 0u, 'a', 3.2f, false);

    // 아래의 코드는 const char*와 int를 포괄하는 공통 타입을 컴파일러가 추론할
    // 수 없어서 컴파일 타임 에러를 만들어낸다.
    // auto data2 = build_array(1, "abc");

    std::string name = "Dongvin";

    // const char* 타입과 std::string 타입이다.
    auto data3 = build_array("Park", name);

    for(auto v : data){
        std::cout << v << " ";
    }
    std::cout << "\n";

    for(auto s : data3){
        std::cout << s << " ";
    }
    std::cout << "\n";
    
    return 0;
}
