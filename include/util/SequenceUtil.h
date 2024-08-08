////////////////////////////////////////////////////////////////////////////////
//
// SequenceUtil.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SEQUENCEUTIL_H_
#define SEQUENCEUTIL_H_
#include <vector>

namespace lightmq { namespace util {

class SequenceUtil {
	SequenceUtil() = delete;
public:
	template<typename T>
	static std::vector<std::pair<T, T>> splitSequence(const std::vector<T>& splitNode, T startSeq, T count);

	template<typename T>
	static std::vector<std::pair<T, T>> getContinuousSequences(const std::vector<T>& sequences);

	template<typename T>
	static std::vector<std::pair<T, T>> subBlocks(T startSeq, T endSeq, const std::vector<std::pair<T, T>>& seqBlocks);

};



// use splitNode split sequece[startSeq, startSeq + count)
template<typename T>
std::vector<std::pair<T, T>> SequenceUtil::splitSequence(const std::vector<T>& splitNode, T startSeq, T count) {
	T endSeq = startSeq + count - 1;
	T lastStartSeq = startSeq;
	std::vector<std::pair<T, T>> splitedSegs;
	for (auto i : splitNode) {
		if (i < startSeq) {
			continue;
		}
		else if (i > endSeq) {
			break;
		}
		if (i == lastStartSeq) {
			++lastStartSeq;
			continue;
		}
		splitedSegs.emplace_back(lastStartSeq, i - 1);
		lastStartSeq = i + 1;
	}
	if (lastStartSeq <= endSeq) {
		splitedSegs.emplace_back(lastStartSeq, endSeq);
	}
	return splitedSegs;
}

template<typename T>
std::vector<std::pair<T, T>> SequenceUtil::getContinuousSequences(const std::vector<T>& sequences) {
	std::vector<std::pair<T, T>> continuousSequences;
	if (sequences.empty()) {
		return continuousSequences;
	}
	continuousSequences.emplace_back(sequences.front(), sequences.front());
	for (std::size_t i = 1; i < sequences.size(); ++i) {
		if (sequences[i] == continuousSequences.back().second + 1) {
			continuousSequences.back().second++;
		}
		else {
			continuousSequences.emplace_back(sequences[i], sequences[i]);
		}
	}
	return continuousSequences;
}


template<typename T>
std::vector<std::pair<T, T>> SequenceUtil::subBlocks(T startSeq, T endSeq, const std::vector<std::pair<T, T>>& seqBlocks){
	std::vector<std::pair<T, T>> res;
	T headSeq = startSeq;
	for(auto& range : seqBlocks){
		if(range.first != headSeq){
			res.emplace_back(headSeq, range.first - 1);
		}
		headSeq = range.second + 1;	
	}
	if(headSeq <= endSeq){
		res.emplace_back(headSeq, endSeq);
	}
	return res;
}

}} // namespace lightmq { namespace util {

#endif  //SEQUENCEUTIL_H_
