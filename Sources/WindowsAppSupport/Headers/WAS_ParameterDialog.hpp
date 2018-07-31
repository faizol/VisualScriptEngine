#ifndef PARAMETERSDIALOG_HPP
#define PARAMETERSDIALOG_HPP

#include "WAS_IncludeWindowsHeaders.hpp"
#include "NUIE_ParameterInterface.hpp"

namespace WAS
{

class ParameterDialog
{
public:
	ParameterDialog (NUIE::ParameterInterfacePtr& paramInterface);

	bool	Show (HWND parent, WORD x, WORD y) const;
	void	OnOkButtonPressed (HWND hwnd);

private:
	NUIE::ParameterInterfacePtr		paramInterface;
	std::vector<std::wstring>		paramValues;
};

}

#endif