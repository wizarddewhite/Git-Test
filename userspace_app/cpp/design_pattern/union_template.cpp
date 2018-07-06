#include <iostream>

// g++ -std=c++11 union_template.cpp

using namespace std;

template<typename StaticVariant>
struct copy_construct
{
   typedef void result_type;
   StaticVariant& sv;
   copy_construct( StaticVariant& s ):sv(s){}
   template<typename T>
   void operator()( const T& v )const
   {
      sv.init(v);
   }
};

template<typename StaticVariant>
struct move_construct
{
   typedef void result_type;
   StaticVariant& sv;
   move_construct( StaticVariant& s ):sv(s){}
   template<typename T>
   void operator()( T& v )const
   {
      sv.init( std::move(v) );
   }
};

template<typename X, typename... Ts>
struct Position;

template<typename X>
struct Position<X> {
    static const int pos = -1;
};

template<typename X, typename... Ts>
struct Position<X, X, Ts...> {
    static const int pos = 0;
};

template<typename X, typename T, typename... Ts>
struct Position<X, T, Ts...> {
    static const int pos = Position<X, Ts...>::pos != -1 ? Position<X, Ts...>::pos + 1 : -1;
};

template<int N, typename... Ts>
struct Storage_Ops;

template<int N, typename T, typename... Ts>
struct Storage_Ops<N, T&, Ts...> {
    static void del(int n, void *data) {}
    static void con(int n, void *data) {}

    template<typename visitor>
    static typename visitor::result_type apply(int n, void *data, visitor& v) {}

    template<typename visitor>
    static typename visitor::result_type apply(int n, void *data, const visitor& v) {}

    template<typename visitor>
    static typename visitor::result_type apply(int n, const void *data, visitor& v) {}

    template<typename visitor>
    static typename visitor::result_type apply(int n, const void *data, const visitor& v) {}
};

template<int N, typename T, typename... Ts>
struct Storage_Ops<N, T, Ts...> {
    static void del(int n, void *data) {
        if(n == N) reinterpret_cast<T*>(data)->~T();
        else Storage_Ops<N + 1, Ts...>::del(n, data);
    }
    static void con(int n, void *data) {
        if(n == N) new(reinterpret_cast<T*>(data)) T();
        else Storage_Ops<N + 1, Ts...>::con(n, data);
    }

    template<typename visitor>
    static typename visitor::result_type apply(int n, void *data, visitor& v) {
        if(n == N) return v(*reinterpret_cast<T*>(data));
        else return Storage_Ops<N + 1, Ts...>::apply(n, data, v);
    }

    template<typename visitor>
    static typename visitor::result_type apply(int n, void *data, const visitor& v) {
        if(n == N) return v(*reinterpret_cast<T*>(data));
        else return Storage_Ops<N + 1, Ts...>::apply(n, data, v);
    }

    template<typename visitor>
    static typename visitor::result_type apply(int n, const void *data, visitor& v) {
        if(n == N) return v(*reinterpret_cast<const T*>(data));
        else return Storage_Ops<N + 1, Ts...>::apply(n, data, v);
    }

    template<typename visitor>
    static typename visitor::result_type apply(int n, const void *data, const visitor& v) {
        if(n == N) return v(*reinterpret_cast<const T*>(data));
        else return Storage_Ops<N + 1, Ts...>::apply(n, data, v);
    }
};

template<int N>
struct Storage_Ops<N> {
    static void del(int n, void *data) {
       //FC_THROW_EXCEPTION( fc::assert_exception, "Internal error: static_variant tag is invalid.");
       throw "Internal error: static_variant tag is invalid.";
    }
    static void con(int n, void *data) {
       //FC_THROW_EXCEPTION( fc::assert_exception, "Internal error: static_variant tag is invalid." );
       throw "Internal error: static_variant tag is invalid.";
    }

    template<typename visitor>
    static typename visitor::result_type apply(int n, void *data, visitor& v) {
       //FC_THROW_EXCEPTION( fc::assert_exception, "Internal error: static_variant tag is invalid." );
       throw "Internal error: static_variant tag is invalid.";
    }
    template<typename visitor>
    static typename visitor::result_type apply(int n, void *data, const visitor& v) {
       //FC_THROW_EXCEPTION( fc::assert_exception, "Internal error: static_variant tag is invalid." );
       throw "Internal error: static_variant tag is invalid.";
    }
    template<typename visitor>
    static typename visitor::result_type apply(int n, const void *data, visitor& v) {
       //FC_THROW_EXCEPTION( fc::assert_exception, "Internal error: static_variant tag is invalid." );
       throw "Internal error: static_variant tag is invalid.";
    }
    template<typename visitor>
    static typename visitor::result_type apply(int n, const void *data, const visitor& v) {
       //FC_THROW_EXCEPTION( fc::assert_exception, "Internal error: static_variant tag is invalid." );
       throw "Internal error: static_variant tag is invalid.";
    }
};

template<typename... Ts>
struct Type_Info;

template<typename T, typename... Ts>
struct Type_Info<T&, Ts...> {
    static const bool no_reference_types = false;
    static const bool no_duplicates = Position<T, Ts...>::pos == -1 && Type_Info<Ts...>::no_duplicates;
    static const size_t size = Type_Info<Ts...>::size > sizeof(T&) ? Type_Info<Ts...>::size : sizeof(T&);
    static const size_t count = 1 + Type_Info<Ts...>::count;
};

template<typename T, typename... Ts>
struct Type_Info<T, Ts...> {
    static const bool no_reference_types = Type_Info<Ts...>::no_reference_types;
    static const bool no_duplicates = Position<T, Ts...>::pos == -1 && Type_Info<Ts...>::no_duplicates;
    static const size_t size = Type_Info<Ts...>::size > sizeof(T) ? Type_Info<Ts...>::size : sizeof(T&);
    static const size_t count = 1 + Type_Info<Ts...>::count;
};

template<>
struct Type_Info<> {
    static const bool no_reference_types = true;
    static const bool no_duplicates = true;
    static const size_t count = 0;
    static const size_t size = 0;
};

template<typename... Types>
class static_variant {
    static_assert(Type_Info<Types...>::no_reference_types, "Reference types are not permitted in static_variant.");
    static_assert(Type_Info<Types...>::no_duplicates, "static_variant type arguments contain duplicate types.");

    int64_t _tag;
    char storage[Type_Info<Types...>::size];

    template<typename X>
    void init(const X& x) {
        _tag = Position<X, Types...>::pos;
        new(storage) X(x);
    }

    template<typename X>
    void init(X&& x) {
        _tag = Position<X, Types...>::pos;
        new(storage) X( std::move(x) );
    }

    template<typename StaticVariant>
    friend struct copy_construct;
    template<typename StaticVariant>
    friend struct move_construct;

public:
    template<typename X>
    struct tag
    {
       static_assert(
         Position<X, Types...>::pos != -1,
         "Type not in static_variant."
       );
       static const int value = Position<X, Types...>::pos;
    };

    int64_t which() const {return _tag;}
    friend bool operator == ( const static_variant& a, const static_variant& b )
    {
       return a.which() == b.which();
    }
    friend bool operator < ( const static_variant& a, const static_variant& b )
    {
       return a.which() < b.which();
    }

    static_variant()
    {
       _tag = 0; //
       Storage_Ops<0, Types...>::con(0, storage);
    }
    ~static_variant() {
       Storage_Ops<0, Types...>::del(_tag, storage);
    }

    template<typename X>
    static_variant(const X& v) {
        static_assert( Position<X, Types...>::pos != -1, "Type not in static_variant.");
        init(v);
    }

    template<typename... Other>
    static_variant( const static_variant<Other...>& cpy )
    {
       cpy.visit( copy_construct<static_variant>(*this) );
    }
    static_variant( const static_variant& cpy )
    {
       cpy.visit( copy_construct<static_variant>(*this) );
    }
    static_variant( static_variant&& mv )
    {
       mv.visit( move_construct<static_variant>(*this) );
    }

    template<typename X>
    static_variant& operator=(const X& v) {
        static_assert(
            Position<X, Types...>::pos != -1, "Type not in static_variant."
        );
        this->~static_variant();
        init(v);
        return *this;
    }
    static_variant& operator=( const static_variant& v )
    {
       if( this == &v ) return *this;
       this->~static_variant();
       v.visit( copy_construct<static_variant>(*this) );
       return *this;
    }
    static_variant& operator=( static_variant&& v )
    {
       if( this == &v ) return *this;
       this->~static_variant();
       v.visit( move_construct<static_variant>(*this) );
       return *this;
    }

    template<typename X>
    X& get() {
        static_assert(
            Position<X, Types...>::pos != -1,
            "Type not in static_variant."
        );
        if(_tag == Position<X, Types...>::pos) {
            void* tmp(storage);
            return *reinterpret_cast<X*>(tmp);
        } else {
            //FC_THROW_EXCEPTION( fc::assert_exception, "static_variant does not contain a value of type ${t}", ("t",fc::get_typename<X>::name()) );
            throw "Internal error: static_variant tag is invalid.";
        }
    }
    template<typename X>
    const X& get() const {
        static_assert(
            Position<X, Types...>::pos != -1,
            "Type not in static_variant."
        );
        if(_tag == Position<X, Types...>::pos) {
            const void* tmp(storage);
            return *reinterpret_cast<const X*>(tmp);
        } else {
            //FC_THROW_EXCEPTION( fc::assert_exception, "static_variant does not contain a value of type ${t}", ("t",fc::get_typename<X>::name()) );
            throw "Internal error: static_variant tag is invalid.";
        }
    }

    template<typename visitor>
    typename visitor::result_type visit(visitor& v) {
        return Storage_Ops<0, Types...>::apply(_tag, storage, v);
    }

    template<typename visitor>
    typename visitor::result_type visit(const visitor& v) {
        return Storage_Ops<0, Types...>::apply(_tag, storage, v);
    }

    template<typename visitor>
    typename visitor::result_type visit(visitor& v)const {
        return Storage_Ops<0, Types...>::apply(_tag, storage, v);
    }

    template<typename visitor>
    typename visitor::result_type visit(const visitor& v)const {
        return Storage_Ops<0, Types...>::apply(_tag, storage, v);
    }
};

struct int_visitor {
	typedef void result_type;
	int content;
	result_type operator()(int c){content = c;}
};

int main()
{
	// sizeof(Type_Info<>) is the largest size of all the type
	cout << "sizeof(Type_Info<int, char>): " << Type_Info<int, char>::size << endl;
	cout << "sizeof(Type_Info<int, char, double>): " << Type_Info<int, char, double>::size << endl;
	cout << endl;

	// pos stands for the position of the first type in following list of
	// types
	cout << "Position of int in <int, int>: " << Position<int, int>::pos << endl;
	cout << "Position of double in <int, char, double>: " << Position<double, int, char, double>::pos << endl;
	cout << endl;

	typedef static_variant<int, char, double> S_Var;
	cout << "char position in <int, char, double>: " << S_Var::tag<char>::value << endl;
	cout << endl;

	// define a static_variant variable
	S_Var a('a'), b(5), c(8);
	if (a == b) {
		cout << "a('a') and b(5) are the same" << endl;
	} else {
		cout << "a('a') and b(5) are different" << endl;
	}
	if (b == c) {
		cout << "b(5) and c(8) are the same" << endl;
	} else {
		cout << "b(5) and c(8) are different" << endl;
	}
	cout << endl;

	char val_a = a.get<char>();
	int val_b = b.get<int>();
	cout << "content in val_a: " << val_a << endl;
	cout << "content in val_b: " << val_b << endl;
	cout << endl;

	// use visitor access content
	int_visitor iv;
	b.visit(iv);
	cout << "access val_b content by visitor: " << iv.content << endl;
	cout << endl;

	// copy construct
	S_Var var_cp(a);
	cout << "content in var_cp(a): " << var_cp.get<char>() << endl;
	// move construct
	S_Var var_mv(S_Var(15));
	cout << "content in var_mv(S_Var(15)): " << var_mv.get<int>() << endl;
	// operator =
	c = var_mv;
	cout << "content in c changed to: " << c.get<int>() << endl;
	cout << endl;
	return 0;
}
