from typing import List

# Leave the whole “solve_bp_decision” function intact
def solve_bp_decision(items: List[float], n_bins: int) -> bool:
    def able_to_pack(items: List[float], bin_capacities: List[float]) -> bool:
        return items == [] or any( 
            able_to_pack( 
                items[:-1], 
                bin_capacities[:i] + [capacity - items[-1]] + bin_capacities[(i + 1):] 
            ) 
            for i, capacity in enumerate(bin_capacities) if capacity >= items[-1] 
        )

    return able_to_pack( sorted(items), [1.0] * n_bins )

# To test this, one can run:
# solve_bp_decision([0.8, 0.09, 0.4, 0.7], 2)
# solve_bp_decision([0.8, 0.09, 0.4, 0.7], 3)


# You should leave function header intact
def solve_bp_evaluation(items: List[float]) -> int:
    for n in range(1, len(items) + 1):
        if solve_bp_decision(items, n):
            return n
    pass


# You should leave function header intact
def solve_bp_search(items: List[float]) -> List[int]:
    bins = [i for i in range(len(items))]
    bins_weight = [items[i] for i in range(len(items))]
    bins_numbers = [bins[i] for i in range(len(items))]
    OPT = solve_bp_evaluation(items)
    while (len(bins_numbers) > OPT):
        for i in range(len(bins_weight)):
            for j in range(i+1, len(bins_weight)):
                if(bins_weight[i] + bins_weight[j] <= 1):
                    new_bins_weight = [bins_weight[i] for i in range(len(bins_weight))]
                    new_bins_weight[i] += new_bins_weight.pop(j)
                    new_bins_number = solve_bp_evaluation(new_bins_weight)
                    if new_bins_number == OPT:
                        bins_weight = new_bins_weight
                        for k in range(len(bins)):
                            if bins[k] == bins_numbers[j]:
                                bins[k] = bins_numbers[i]
                        bins_numbers.pop(j)
                        break
    free = 1
    new_numbers = {}
    for k in range(len(bins)):
        if not (bins[k] in new_numbers): 
            new_numbers[bins[k]] = free
            free += 1
        bins[k] = new_numbers[bins[k]]
    return bins