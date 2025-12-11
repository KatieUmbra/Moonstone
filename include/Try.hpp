#pragma once
#include <expected>
#include <utility>
// https://www.reddit.com/r/rust/comments/1he8xf6/comment/m22hoxk/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
#define Try(EXPR)                                                              \
	({                                                                         \
		auto&& _temporary_result = (EXPR);                                     \
		if (!_temporary_result.has_value()) [[unlikely]]                       \
			return std::unexpected(std::move(_temporary_result).error());      \
		*std::move(_temporary_result);                                         \
	})
