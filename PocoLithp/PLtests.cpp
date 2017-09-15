#include "stdafx.h"

namespace PocoLithp {
	namespace Test {
		void plithp_test() {
			std::string line;
			LithpEnvironment _env, *env = &_env; add_globals(_env);
			auto start = std::chrono::steady_clock::now();
			evalTimed(read("(define multiply-by (lambda (n) (lambda (y) (* y n))))"), env);
			evalTimed(read("(define doubler (multiply-by 2))"), env);
			auto step1 = std::chrono::steady_clock::now();
			LithpCell result = evalTimed(read("(doubler 4)"), env);
			auto step2 = std::chrono::steady_clock::now();
			std::cout << to_string(result) << "\n";
			std::cout << "parse time: " << std::chrono::duration_cast<std::chrono::milliseconds>(step1 - start).count() << "ms\n";
			std::cout << "run time: " << std::chrono::duration_cast<std::chrono::milliseconds>(step2 - step1).count() << "ms\n";
		}

		void plithp_fac_test() {
			std::string line;
			LithpEnvironment _env, *env = &_env; add_globals(_env);
			auto start = std::chrono::steady_clock::now();
			LithpCell result = evalTimed(read(
				"(begin "
				"  (define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))"
				"  (fact 50)"
				")"), env);
			auto step1 = std::chrono::steady_clock::now();
			std::cout << "(fact 50) => " << to_string(result) << "\n";
			std::cout << "  fact run time: " << std::chrono::duration_cast<std::chrono::milliseconds>(step1 - start).count() << "ms\n";
		}

		void plithp_fib_test() {
			// Test a fibonacci function
			// ((def fib #N :: (
			//    (if (< N 2) (
			//      (1)
			//    ) (else (
			//      (+ (fib (- N 1)) (fib (- N 2)))
			//    )))
			//  ))
			//  (var X 10)
			//  (print "Fac of" (get X) (fac (get X)))
			// )
			std::string line;
			auto start = std::chrono::steady_clock::now();
			LithpEnvironment _env, *env = &_env; add_globals(_env);
			const LithpCell &result = evalTimed(read(
				"(begin "
				"  (define fib (lambda (n) (if (< n 2) 1 (+ (fib (- n 1)) (fib (- n 2))))))"
				"  (fib 12)"
				")"), env);
			auto step1 = std::chrono::steady_clock::now();
			std::cout << "(fib 12) => " << to_string(result) << "\n";
			std::cout << "  fib run time: " << std::chrono::duration_cast<std::chrono::milliseconds>(step1 - start).count() << "ms\n";
		}
		////////////////////// unit tests
		unsigned g_test_count;      // count of number of unit tests executed
		unsigned g_fault_count;     // count of number of unit tests that fail
		template <typename T1, typename T2>
		void test_equal_(const T1 & value, const T2 & expected_value, const char * file, int line, const std::string &code)
		{
			++g_test_count;
			std::cerr
				<< code << " : "
				<< " expected " << expected_value
				<< ", got " << value;
			if (value != expected_value) {
				++g_fault_count;
				std::cerr << " - FAIL\n";
			} else {
				std::cerr << " - success\n";
			}
		}
		// write a message to std::cout if value != expected_value
#define TEST_EQUAL(value, expected_value, code) test_equal_(value, expected_value, __FILE__, __LINE__, code)
// evalTimeduate the given Lisp expression and compare the result against the given expected_result
#define TEST(expr, expected_result) TEST_EQUAL(to_string(evalTimed(read(expr), &global_env)), expected_result, expr)

		unsigned plithp_abs_test() {
			LithpEnvironment global_env; add_globals(global_env);
			TEST("(define abs (lambda (n) ((if (> n 0) + -) 0 n)))", "<Lambda>");
			TEST("(list (abs -3) (abs 0) (abs 3))", "(3 0 3)");
			return 0;
		}

		unsigned plithp_complete_test() {
			LithpEnvironment global_env; add_globals(global_env);
			auto start = std::chrono::steady_clock::now();
			// the 29 unit tests for lis.py
			TEST("(< 10 2)", "#f");
			TEST("(<= 10 2)", "#f");
			TEST("(quote \"f\\\"oo\")", "f\\\"oo");
			TEST("(quote \"foo\")", "foo");
			TEST("(quote (testing 1 (2.0) -3.14e159))", "(testing 1 (2) -3.14e+159)");
			TEST("(+ 2 2)", "4");
			TEST("(+ 2.5 2)", "4.5");
			TEST("(+ (* 2 100) (* 1 10))", "210");
			TEST("(> 6 5)", "#t");
			TEST("(< 6 5)", "#f");
			TEST("(if (> 6 5) (+ 1 1) (+ 2 2))", "2");
			TEST("(if (< 6 5) (+ 1 1) (+ 2 2))", "4");
			TEST("(define x 3)", "3");
			TEST("x", "3");
			TEST("(+ x x)", "6");
			TEST("(begin (define x 1) (set! x (+ x 1)) (+ x 1))", "3");
			TEST("((lambda (x) (+ x x)) 5)", "10");
			TEST("(define twice (lambda (x) (* 2 x)))", "<Lambda>");
			TEST("(twice 5)", "10");
			TEST("(define compose (lambda (f g) (lambda (x) (f (g x)))))", "<Lambda>");
			TEST("((compose list twice) 5)", "(10)");
			TEST("(define repeat (lambda (f) (compose f f)))", "<Lambda>");
			TEST("((repeat twice) 5)", "20");
			TEST("((repeat (repeat twice)) 5)", "80");
			TEST("(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))", "<Lambda>");
			TEST("(fact 3)", "6");
			// TODO: Bignum support
			//TEST("(fact 50)", "30414093201713378043612608166064768844377641568960512000000000000");
			TEST("(fact 12)", "479001600");
			TEST("(define abs (lambda (n) ((if (> n 0) + -) 0 n)))", "<Lambda>");
			TEST("(list (abs -3) (abs 0) (abs 3))", "(3 0 3)");
			TEST("(define combine (lambda (f)"
				"(lambda (x y)"
				"(if (null? x) (quote ())"
				"(f (list (head x) (head y))"
				"((combine f) (tail x) (tail y)))))))", "<Lambda>");
			TEST("(define zip (combine cons))", "<Lambda>");
			TEST("(zip (list 1 2 3 4) (list 5 6 7 8))", "((1 5) (2 6) (3 7) (4 8))");
			TEST("(define riff-shuffle (lambda (deck) (begin"
				"(define take (lambda (n seq) (if (<= n 0) (quote ()) (cons (head seq) (take (- n 1) (tail seq))))))"
				"(define drop (lambda (n seq) (if (<= n 0) seq (drop (- n 1) (tail seq)))))"
				"(define mid (lambda (seq) (/ (length seq) 2)))"
				"((combine append) (take (mid deck) deck) (drop (mid deck) deck)))))", "<Lambda>");
			TEST("(riff-shuffle (list 1 2 3 4 5 6 7 8))", "(1 5 2 6 3 7 4 8)");
			TEST("((repeat riff-shuffle) (list 1 2 3 4 5 6 7 8))", "(1 3 5 7 2 4 6 8)");
			TEST("(riff-shuffle (riff-shuffle (riff-shuffle (list 1 2 3 4 5 6 7 8))))", "(1 2 3 4 5 6 7 8)");
			auto end = std::chrono::steady_clock::now();
			std::cout
				<< "total tests " << g_test_count
				<< ", total failures " << g_fault_count
				<< ", run in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
			return g_fault_count ? EXIT_FAILURE : EXIT_SUCCESS;
		}

		int RunTests() {
			//plithp_test();
			//plithp_abs_test();
			//plithp_fac_test();
			//plithp_fib_test();
			plithp_complete_test();
			std::cout << "Reductions: " << reductions << std::endl;
			return g_fault_count;
		}
	}
}
