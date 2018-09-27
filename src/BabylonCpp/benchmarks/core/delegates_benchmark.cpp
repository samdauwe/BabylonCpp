#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>

#include <babylon/core/delegates/delegate.h>

typedef uint64_t ns;
typedef uint32_t rep;

class Benchmark {

public:
  static void Run()
  {
    // 20000, 10000
    OperationSet<10000, 10000, 10000, SA::delegate<double(int)>, double, int>
      set;
    set.Run(11);
  } // Run

private:
  template <size_t delegate_count, rep creation_repeat_count, rep call_count,
            typename DELEGATE, typename RET, typename... PARAMS>
  class OperationSet {
  private:
    ns DelegateCreation()
    {
      Sample sample;
      Start();
      for (rep repeat_index = 0; repeat_index < creation_repeat_count;
           ++repeat_index)
        for (DELEGATE* j = delegates; j != delegates + delegate_count; ++j)
          *j = DELEGATE::template create<Sample, &Sample::A>(&sample);
      return Stop();
    } // DelegateCreation
    ns LambdaDelegateCreation()
    {
      auto lambda = [](PARAMS...) -> RET { return 0.0; };
      Start();
      for (rep repeat_index = 0; repeat_index < creation_repeat_count;
           ++repeat_index)
        for (DELEGATE* j = delegates; j != delegates + delegate_count; ++j)
          *j = SA::delegate<RET(PARAMS...)>::template create<decltype(lambda)>(
            lambda);
      return Stop();
    } // LambdaDelegateCreation
    ns DelegateCall(PARAMS... arg)
    {
      Start();
      for (size_t delegate = 0; delegate < delegate_count; ++delegate)
        for (size_t index = 0; index < call_count; ++index)
          delegates[delegate](arg...);
      return Stop();
    } // DelegateCall
    ns FunctionCreation()
    {
      Sample sample;
      using namespace std::placeholders;
      Start();
      for (rep repeat_index = 0; repeat_index < creation_repeat_count;
           ++repeat_index)
        for (std::function<RET(PARAMS...)>* j = functions;
             j != functions + delegate_count; ++j)
          *j = std::bind(&Sample::A, &sample, _1);
      return Stop();
    } // FunctionCreation
    ns LambdaFunctionCreation()
    {
      auto lambda = [](PARAMS...) -> RET { return 0.0; };
      Start();
      for (unsigned int repeat_index = 0; repeat_index < creation_repeat_count;
           ++repeat_index)
        for (std::function<RET(PARAMS...)>* j = functions;
             j != functions + delegate_count; ++j)
          *j = std::function<RET(PARAMS...)>(lambda);
      return Stop();
    } // LambdaFunctionCreation
    ns FunctionCall(PARAMS... arg)
    {
      Start();
      for (size_t fun = 0; fun < delegate_count; ++fun)
        for (size_t index = 0; index < call_count; ++index)
          functions[fun](arg...);
      return Stop();
    } // FunctionCall

  public:
    void Run(PARAMS... arg)
    {
      functionCreationTime = FunctionCreation();
      functionCallTime     = FunctionCall(arg...);
      delegateCreationTime = DelegateCreation();
      delegateCallTime     = DelegateCall(arg...);
      // lambda:
      lambdaFunctionCreationTime = LambdaFunctionCreation();
      lambdaFunctionCallTime     = FunctionCall(arg...);
      lambdaDelegateCreationTime = LambdaDelegateCreation();
      lambdaDelegateCallTime     = DelegateCall(arg...);
      Show();
    } // Run

  private:
    DELEGATE delegates[delegate_count];
    std::function<RET(PARAMS...)> functions[delegate_count];
    ns delegateCreationTime, lambdaDelegateCreationTime, delegateCallTime,
      lambdaDelegateCallTime, functionCreationTime, lambdaFunctionCreationTime,
      functionCallTime, lambdaFunctionCallTime;
    void Show()
    {
      auto comparison = [&](ns creationTime1, ns callTime1, ns creationTime2,
                            ns callTime2, const char* title) -> void {
        ns total1 = creationTime1 + callTime1;
        ns total2 = creationTime2 + callTime2;
        auto averageCreation1
          = 1.0 * creationTime1 / (delegate_count * creation_repeat_count);
        auto averageCreation2
          = 1.0 * creationTime2 / (delegate_count * creation_repeat_count);
        auto averageCall1 = 1.0 * callTime1 / (delegate_count * call_count);
        auto averageCall2 = 1.0 * callTime2 / (delegate_count * call_count);
        std::cout << title << ":" << std::endl;
        std::cout << "\tCreation: " << creationTime1 << " vs. " << creationTime2
                  << std::endl;
        std::cout << "\tAverage creation: " << averageCreation1 << " vs. "
                  << averageCreation2 << std::endl;
        std::cout << "\tCall: " << callTime1 << " vs. " << callTime2
                  << std::endl;
        std::cout << "\tAverage call: " << averageCall1 << " vs. "
                  << averageCall2 << std::endl;
        std::cout << "\tTotal: " << total1 << " vs. " << total2 << std::endl;
        std::cout << "\tCreation gain:\t" << 1.0 * creationTime2 / creationTime1
                  << std::endl;
        std::cout << "\tCall gain:\t" << 1.0 * callTime2 / callTime1
                  << std::endl;
        std::cout << "\tTotal gain:\t" << 1.0 * total2 / total1 << std::endl;
      };
      comparison(delegateCreationTime, delegateCallTime, functionCreationTime,
                 functionCallTime,
                 "Instance methods of a class, delegate vs. std::function");
      comparison(lambdaDelegateCreationTime, lambdaDelegateCallTime,
                 lambdaFunctionCreationTime, lambdaFunctionCallTime,
                 "Lambda functions, delegate vs. std::function");
    } // Show
    std::chrono::high_resolution_clock clock;
    std::chrono::high_resolution_clock::time_point before;
    void Start()
    {
      before = clock.now();
    }
    ns Stop()
    {
      auto after = clock.now();
      auto duration
        = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before)
            .count();
      return static_cast<ns>(duration);
    } // Stop
  };  // OperationSet

  class Sample {
  public:
    double A(int)
    {
      return 0.1;
    }
    static double B(int)
    {
      return 0.2;
    }
  }; // class Sample

}; /* class Benchmark */

TEST(BenchmarkDelegates, delegate)
{
  Benchmark::Run();
}
