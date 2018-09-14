#ifndef _K_MEANS_HPP
#define _K_MEANS_HPP
#include <functional>
#include <type_traits>
#include <vector>
#include <cassert>
#include <random>
#include <set>

namespace RSY_TOOL::K_means
{

	enum class copy_strategy { NO_STEAL, STEAL };


	/*
	 * Parameter:
	 *     _D            :  sample set.
	 *     cluster_amount:  the expected amount of clusters.
	 *     dist          :  functor that compute the distance between two Vec.
	 *
	 * Template parameter:
	 *     Dist  :  should be comparable.(use `<`, return bool or something convertible to bool)
	 *     Vec   :  support `+=`, `/=`, `==`, default constructor, copy (NO_STEAL) or move (STEAL)
	 *
	 * Return value:
	 *     vector which has cluster_amount sets which represent different cluster.
	 *
	 * Specification:
	 *     _D's type is non-const reference since parameter maybe rvalue(initializer-list),
	 *    which should not be called in this function.
	 */
	template<
		typename Vec,
		typename Dist,
		std::enable_if_t<std::is_convertible_v<decltype(std::declval<Dist>() < std::declval<Dist>()), bool>
		&& std::is_same_v<std::decay_t<decltype(std::declval<Vec>() += std::declval<Vec>())>, Vec>
		&& std::is_same_v<std::decay_t<decltype(std::declval<Vec>() /= static_cast<std::size_t>(1u))>, Vec>
		&& std::is_convertible_v<decltype(std::declval<Vec>() == std::declval<Vec>()), bool>
		>* = nullptr
		> std::vector<std::set<std::size_t>> k_means_return_label(std::vector<Vec>& _D, const std::size_t cluster_amount, const std::function<Dist(const Vec&, const Vec&)> dist)
	{
		const std::vector<Vec>& D = _D;
		const std::size_t size = D.size();
		assert((size >= cluster_amount));

		std::vector<Vec> mean_vecs;
		mean_vecs.reserve(cluster_amount);

		//generate random num to initialize the mean_vecs.
		std::set<std::size_t> random_num;
		std::default_random_engine re{ std::random_device{}() };
		while (random_num.size() < cluster_amount)
		{
			random_num.insert(std::uniform_int_distribution<std::size_t>{0, size - 1}(re));
		}
		for (auto r : random_num)
			mean_vecs.push_back(D[r]);

		/*
		 * Divide vecs into clusters according to their distance,
		 * and calculate the new mean_vec.
		 * Loop until mean_vecs does not change.
		 * Note Bene: C[label] represents the cluster that respondes to mean_vecs[label].
		 */
		bool isChanged = true;
		std::vector<std::set<std::size_t>> C(cluster_amount);
		/*
		 * Other Condition that control the loop times,
		 * which can be customized varying the situations.
		 */
		bool other_condition = false;
		while (isChanged)
		{
			isChanged = false;
			for (auto& c : C)
				c.clear();

			//for each vec in D, divide it into one set of C.
			for (std::size_t i = 0; i < size; i++)
			{
				const Vec& vec = D[i];
				std::size_t label = 0;
				Dist _dist = dist(vec, mean_vecs[0]);
				for (std::size_t j = 1; j < cluster_amount; j++)
				{
					Dist _cur_dist = dist(vec, mean_vecs[j]);
					if (_cur_dist < _dist)
					{
						_dist = std::move(_cur_dist);
						label = j;
					}
				}
				C[label].insert(i);
			}

			//update mean_vecs, and set up `isChanged` flag if needed.
			for (std::size_t label = 0; label < cluster_amount; label++)
			{
				Vec mean_vec{};
				for (auto i : C[label])
				{
					mean_vec += D[i];
				}
				mean_vec /= C[label].size();
				if (!(mean_vec == mean_vecs[label]))
				{
					mean_vecs[label] = std::move(mean_vec);
					isChanged = true;
				}
			}

			if (other_condition) break;

		}

		return C;

	}


	/*
	 * Parameter:
	 *     _D            :  sample set.
	 *     cluster_amount:  the expected amount of clusters.
	 *     dist          :  functor that compute the distance between two Vec.
	 *
	 * Template parameter:
	 *     Dist  :  should be comparable.(use `<`, return bool or something convertible to bool)
	 *     Vec   :  support `+=`, `/=`, `==`, default constructor, copy (NO_STEAL) or move (STEAL)
	 *
	 * Return value:
	 *     vector which has cluster_amount sets which represent different cluster.
	 */
	template<
		typename Vec,
		typename Dist,
		std::enable_if_t<std::is_convertible_v<decltype(std::declval<Dist>() < std::declval<Dist>()), bool>
		&& std::is_same_v<std::decay_t<decltype(std::declval<Vec>() += std::declval<Vec>())>, Vec>
		&& std::is_same_v<std::decay_t<decltype(std::declval<Vec>() /= static_cast<std::size_t>(1u))>, Vec>
		&& std::is_convertible_v<decltype(std::declval<Vec>() == std::declval<Vec>()), bool>
		>* = nullptr
		> std::vector<std::vector<Vec>> k_means(const std::vector<Vec>& D, const std::size_t cluster_amount, const std::function<Dist(const Vec&, const Vec&)> dist, const copy_strategy _copy_strategy = copy_strategy::NO_STEAL)
	{
		std::vector<std::vector<Vec>> clusters(cluster_amount);
		std::vector<std::set<std::size_t>> labels = k_means_return_label(const_cast<std::vector<Vec>&>(D), cluster_amount, dist);
		auto c_it = clusters.begin();
		for (auto l_it = labels.begin(); l_it != labels.end(); ++l_it, ++c_it)
		{
			c_it->reserve(l_it->size());
			for (auto label : *l_it)
			{
				if (copy_strategy::STEAL == _copy_strategy)
					c_it->push_back(std::move((const_cast<Vec&>(D[label]))));
				else c_it->push_back(D[label]);
			}
		}
		return clusters;
	}


}//end namespace RSY_TOOL::K_means

#endif // !_K_MEANS_HPP
