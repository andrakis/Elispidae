#include "stdafx.h"

namespace PocoLithp {
	namespace Test {
		void plithp_test() {
			std::string line;
			//LithpEnvironment _env, *env = &_env; add_globals(_env);
			LithpEnvironment *env_obj = new LithpEnvironment();
			add_globals(*env_obj);
			Env_p env(env_obj);
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
			//LithpEnvironment _env, *env = &_env; add_globals(_env);
			LithpEnvironment *env_obj = new LithpEnvironment();
			add_globals(*env_obj);
			Env_p env(env_obj);
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
			//LithpEnvironment _env, *env = &_env; add_globals(_env);
			LithpEnvironment *env_obj = new LithpEnvironment();
			add_globals(*env_obj);
			Env_p env(env_obj);
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
// evaluate the given Lisp expression and compare the result against the given expected_result
#define TEST(expr, expected_result) TEST_EQUAL(to_string(evalTimed(read(expr), global_env)), expected_result, expr)

		unsigned plithp_abs_test() {
			//LithpEnvironment global_env; add_globals(global_env);
			LithpEnvironment *global_env_obj = new LithpEnvironment();
			add_globals(*global_env_obj);
			Env_p global_env(global_env_obj);
			TEST("(define abs (lambda (n) ((if (> n 0) + -) 0 n)))", "<Lambda>");
			TEST("(list (abs -3) (abs 0) (abs 3))", "(3 0 3)");
			return 0;
		}

		unsigned plithp_complete_test() {
			//LithpEnvironment global_env; add_globals(global_env);
			LithpEnvironment *global_env_obj = new LithpEnvironment();
			add_globals(*global_env_obj);
			Env_p global_env(global_env_obj);
			auto start = std::chrono::steady_clock::now();
			// the 29 unit tests for lis.py
			TEST("((lambda (X) (+ X X)) 5)", "10");
			TEST("(< 10 2)", "false");
			TEST("(<= 10 2)", "false");
			TEST("(quote \"f\\\"oo\")", "f\\\"oo");
			TEST("(quote \"foo\")", "foo");
			TEST("(quote (testing 1 (2.0) -3.14e159))", "(testing 1 (2.000000e+00) -3.140000e+159)");
			TEST("(+ 2 2)", "4");
			TEST("(+ 2.5 2)", "4.500000e+00");
			TEST("(* 2.25 2)", "4.500000e+00");    // Bugfix, multiplication was losing floating point value
			TEST("(+ (* 2 100) (* 1 10))", "210");
			TEST("(> 6 5)", "true");
			TEST("(< 6 5)", "false");
			TEST("(if (> 6 5) (+ 1 1) (+ 2 2))", "2");
			TEST("(if (< 6 5) (+ 1 1) (+ 2 2))", "4");
			TEST("(define X 3)", "3");
			TEST("X", "3");
			TEST("(+ X X)", "6");
			TEST("(begin (define X 1) (set! X (+ X 1)) (+ X 1))", "3");
			TEST("(define twice (lambda (X) (* 2 X)))", "<Lambda>");
			TEST("(twice 5)", "10");
			TEST("(define compose (lambda (F G) (lambda (X) (F (G X)))))", "<Lambda>");
			TEST("((compose list twice) 5)", "(10)");
			TEST("(define repeat (lambda (F) (compose F F)))", "<Lambda>");
			TEST("((repeat twice) 5)", "20");
			TEST("((repeat (repeat twice)) 5)", "80");
			// Factorial - head recursive
			TEST("(define fact (lambda (N) (if (<= N 1) 1 (* N (fact (- N 1))))))", "<Lambda>");
			TEST("(fact 3)", "6");
			// TODO: Bignum support
			TEST("(fact 12)", "479001600");
			// Factorial - tail recursive
			TEST("(begin (define fac (# (N) (fac2 N 1))) (define fac2 (# (N A) (if (<= N 0) A (fac2 (- N 1) (* N A))))))", "<Lambda>");
			TEST("(fac 50.1)", "4.732679e+63");   // Bugfix, multiplication was losing floating point value
			TEST("(define abs (lambda (N) ((if (> N 0) + -) 0 N)))", "<Lambda>");
			TEST("(list (abs -3) (abs 0) (abs 3))", "(3 0 3)");
			TEST("(define combine (lambda (F)"
				"(lambda (X Y)"
				"(if (null? X) (quote ())"
				"(F (list (head X) (head Y))"
				"((combine F) (tail X) (tail Y)))))))", "<Lambda>");
			TEST("(define zip (combine cons))", "<Lambda>");
			TEST("(zip (list 1 2 3 4) (list 5 6 7 8))", "((1 5) (2 6) (3 7) (4 8))");
			TEST("(define riff-shuffle (lambda (Deck) (begin"
				"(define take (lambda (N Seq) (if (<= N 0) (quote ()) (cons (head Seq) (take (- N 1) (tail Seq))))))"
				"(define drop (lambda (N Seq) (if (<= N 0) Seq (drop (- N 1) (tail Seq)))))"
				"(define mid (lambda (Seq) (/ (length Seq) 2)))"
				"((combine (get! append)) (take (mid Deck) Deck) (drop (mid Deck) Deck)))))", "<Lambda>");
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
			return g_fault_count;
		}
	}
}
