#pragma once
#include <vector>
#include <tuple>
#include "rxcpp/rx.hpp"

namespace rxcppextras_internal
{
    template <typename T>
    class CombineLatestAccumulator
    {
    public:
        std::vector<T> latestValues;
        std::vector<bool> valueSeen;
        CombineLatestAccumulator(int size)
        {
            latestValues = std::vector<T>(size);
            valueSeen = std::vector<bool>(size);
        }
    };
} // namespace rxcppextras_internal
namespace rxcppextras
{
    template <typename T>
    rxcpp::observable<std::vector<T>> combine_latest(std::vector<rxcpp::observable<T>> inputs)
    {
        auto inputSize = inputs.size();
        rxcppextras_internal::CombineLatestAccumulator<T> accumulator(inputSize);
        // Merge operating on vector and providing a projection to include index
        return rxcpp::observable<>::create<std::tuple<int, T>>(
                   [inputs, inputSize](rxcpp::subscriber<std::tuple<int, T>> obs) {
                       // Project inner streams into a tuple of index number & value
                       for (int i = 0; i < inputSize; i++)
                       {
                           auto innerStream = inputs[i];
                           // Project with an index number
                           auto proxySubscription = innerStream
                                                        .map([i](bool value) {
                                                            return std::tuple<int, T>(i, value);
                                                        })
                                                        .subscribe(obs);
                           // Add disposal
                           obs.add([proxySubscription]() {
                               proxySubscription.unsubscribe();
                           });
                       }
                   })
            // Accumulate to get latest values and indication that propagation has happened
            .scan(accumulator,
                  [](rxcppextras_internal::CombineLatestAccumulator<T> acc, std::tuple<int, T> curr) -> CombineLatestAccumulator<T> {
                      auto currentIndex = std::get<0>(curr);
                      auto currentValue = std::get<1>(curr);
                      // Update state vector
                      acc.latestValues[currentIndex] = currentValue;
                      // And set "seen flag"
                      acc.valueSeen[currentIndex] = true;
                      return acc;
                  })
            .filter([](rxcppextras_internal::CombineLatestAccumulator<T> x) -> bool {
                // Only propagate if all Values seen
                auto count = 0;
                for (auto i : x.valueSeen)
                {
                    if (i)
                    {
                        count++;
                    }
                }
                return count == x.valueSeen.size();
            })
            .map([](rxcppextras_internal::CombineLatestAccumulator<T> x) -> std::vector<T> {
                return x.latestValues;
            });
    }
    template <typename T, typename TOutput>
    rxcpp::observable<TOutput> combine_latest(std::vector<rxcpp::observable<T>> inputs,
                                              std::function<TOutput(std::vector<T>)> projectionFunction)
    {
        return rxcppextras::combine_latest<T>(inputs)
            .map([projectionFunction](std::vector<T> latest) -> TOutput {
                return projectionFunction(latest);
            });
    }
} // namespace rxcppextras