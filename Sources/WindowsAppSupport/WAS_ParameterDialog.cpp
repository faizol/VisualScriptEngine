#include "WAS_ParameterDialog.hpp"

#include "NUIE_NodeParameters.hpp"
#include "NE_SingleValues.hpp"
#include "NE_Localization.hpp"
#include "NE_Debug.hpp"

#include <string>

namespace WAS
{

static const short DialogPadding = 5;
static const short StaticWidth = 60;
static const short ControlWidth = 150;
static const short ControlHeight = 12;
static const short ButtonWidth = 50;
static const short ButtonHeight = 15;

static const DWORD FirstControlId = 10000;
static const DWORD StaticControlIdOffset = 1000;
static const DWORD SeparatorId = 2000;
static const DWORD CancelButtonId = 2001;
static const DWORD OkButtonId = 2002;

static DWORD ParamIdToControlId (size_t paramId)
{
	return FirstControlId + (DWORD) paramId;
}

static size_t ControlIdToParamId (DWORD controlId)
{
	return (size_t) controlId - FirstControlId;
}

INT_PTR CALLBACK DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			{
				SetWindowLongPtr (hwnd, GWLP_USERDATA, lParam);
				ParameterDialog* paramDialog = (ParameterDialog*) lParam;
				if (paramDialog != nullptr) {
					paramDialog->SetupControls (hwnd);
				}
			}
			break;
		case WM_CLOSE:
			{
				EndDialog (hwnd, 0);
			}
			break;
		case WM_COMMAND:
			{
				ParameterDialog* paramDialog = (ParameterDialog*) GetWindowLongPtr (hwnd, GWLP_USERDATA);
				if (paramDialog == nullptr) {
					break;
				}

				WORD commandId = LOWORD (wParam);
				if (commandId == OkButtonId) {
					if (paramDialog->CollectChangedValues (hwnd)) {
						EndDialog (hwnd, 1);
					}
				} else if (commandId == CancelButtonId) {
					EndDialog (hwnd, 0);
				} else {
					WORD notificationCode = HIWORD (wParam);
					switch (notificationCode) {
						case EN_CHANGE:
						case CBN_SELCHANGE:
							{
								DWORD controlId = GetWindowLong ((HWND) lParam, GWL_ID);
								paramDialog->SetParameterChanged (controlId);
							}
							break;
					}
				}
			}
			break;
	}

	return 0;
}

ParameterDialog::ChangedParameter::ChangedParameter (size_t index, const NE::ValuePtr& value) :
	index (index),
	value (value)
{

}

size_t ParameterDialog::ChangedParameter::GetIndex () const
{
	return index;
}

const NE::ValuePtr& ParameterDialog::ChangedParameter::GetValue () const
{
	return value;
}

ParameterDialog::ParameterDialog (NUIE::ParameterInterfacePtr& paramInterface) :
	paramInterface (paramInterface),
	inMemoryDialog (nullptr),
	isSetUp (false)
{

}

bool ParameterDialog::Show (HWND parent, short x, short y)
{
	WORD paramCount = (WORD) paramInterface->GetParameterCount ();
	short dialogInnerWidth = StaticWidth + ControlWidth + DialogPadding;
	short dialogWidth = dialogInnerWidth + 2 * DialogPadding;
	short dialogHeight = paramCount * ControlHeight + (paramCount + 3) * DialogPadding + ButtonHeight;
	InMemoryDialog dialog (NE::LocalizeString (L"Parameters"), x, y, dialogWidth, dialogHeight);

	short currentY = DialogPadding;
	for (WORD paramIndex = 0; paramIndex < paramCount; ++paramIndex) {
		DWORD controlId = ParamIdToControlId (paramIndex);

		NUIE::ParameterType type = paramInterface->GetParameterType (paramIndex);
		dialog.AddStatic (paramInterface->GetParameterName (paramIndex).c_str (), DialogPadding, currentY, StaticWidth, ControlHeight, controlId + StaticControlIdOffset);

		NE::ValueConstPtr value = paramInterface->GetParameterValue (paramIndex);
		if (type == NUIE::ParameterType::Boolean) {
			if (DBGVERIFY (NE::Value::IsType<NE::BooleanValue> (value))) {
				int selectedChoice = NE::BooleanValue::Get (value) ? 0 : 1;
				std::vector<std::wstring> choices = { NE::LocalizeString (L"true"), NE::LocalizeString (L"false") };
				dialog.AddComboBox (selectedChoice, choices, StaticWidth + 2 * DialogPadding, currentY, ControlWidth, ControlHeight, controlId);
			}
		} else if (type == NUIE::ParameterType::Integer) {
			if (DBGVERIFY (NE::Value::IsType<NE::IntValue> (value))) {
				std::wstring controlText = NUIE::ParameterValueToString (value, type);
				dialog.AddEdit (controlText.c_str (), StaticWidth + 2 * DialogPadding, currentY, ControlWidth, ControlHeight, controlId);
			}
		} else if (type == NUIE::ParameterType::Float) {
			if (DBGVERIFY (NE::Value::IsType<NE::FloatValue> (value))) {
				std::wstring controlText = NUIE::ParameterValueToString (value, type);
				dialog.AddEdit (controlText.c_str (), StaticWidth + 2 * DialogPadding, currentY, ControlWidth, ControlHeight, controlId);
			}
		} else if (type == NUIE::ParameterType::Double) {
			if (DBGVERIFY (NE::Value::IsType<NE::DoubleValue> (value))) {
				std::wstring controlText = NUIE::ParameterValueToString (value, type);
				dialog.AddEdit (controlText.c_str (), StaticWidth + 2 * DialogPadding, currentY, ControlWidth, ControlHeight, controlId);
			}
		} else if (type == NUIE::ParameterType::String) {
			if (DBGVERIFY (NE::Value::IsType<NE::StringValue> (value))) {
				std::wstring controlText = NUIE::ParameterValueToString (value, type);
				dialog.AddEdit (controlText.c_str (), StaticWidth + 2 * DialogPadding, currentY, ControlWidth, ControlHeight, controlId);
			}
		} else if (type == NUIE::ParameterType::Enumeration) {
			if (DBGVERIFY (NE::Value::IsType<NE::IntValue> (value))) {
				int selectedChoice = NE::IntValue::Get (value);
				std::vector<std::wstring> choices = paramInterface->GetParameterValueChoices (paramIndex);
				dialog.AddComboBox (selectedChoice, choices, StaticWidth + 2 * DialogPadding, currentY, ControlWidth, ControlHeight, controlId);
			}
		} else {
			DBGBREAK ();
		}

		currentY += ControlHeight + DialogPadding;
	}

	dialog.AddSeparator (DialogPadding, currentY, dialogInnerWidth + 2, SeparatorId);
	dialog.AddButton (NE::LocalizeString (L"Cancel"), dialogInnerWidth - 2 * ButtonWidth, currentY + DialogPadding, ButtonWidth, ButtonHeight, CancelButtonId);
	dialog.AddDefButton (NE::LocalizeString (L"OK"), dialogInnerWidth - ButtonWidth + DialogPadding, currentY + DialogPadding, ButtonWidth, ButtonHeight, OkButtonId);

	inMemoryDialog = &dialog;
	if (dialog.Show (parent, DlgProc, (LPARAM) this) == 1) {
		ApplyParameterChanges ();
		return true;
	}
	inMemoryDialog = nullptr;

	return false;
}

void ParameterDialog::SetupControls (HWND dialogHwnd)
{
	if (DBGVERIFY (inMemoryDialog != nullptr)) {
		inMemoryDialog->SetupControls (dialogHwnd);
		isSetUp = true;
	}
}

void ParameterDialog::SetParameterChanged (DWORD controlId)
{
	if (!isSetUp) {
		return;
	}
	changedParams.insert (ControlIdToParamId (controlId));
}

bool ParameterDialog::CollectChangedValues (HWND hwnd)
{
	bool isAllParameterValid = true;
	for (size_t paramId = 0; paramId < paramInterface->GetParameterCount (); ++paramId) {
		if (changedParams.find (paramId) == changedParams.end ()) {
			continue;
		}

		DWORD controlId = ParamIdToControlId (paramId);

		NUIE::ParameterType type = paramInterface->GetParameterType (paramId);

		NE::ValuePtr value;
		if (type == NUIE::ParameterType::Boolean) {
			HWND comboBoxHwnd = GetDlgItem (hwnd, controlId);
			LRESULT selected = SendMessage (comboBoxHwnd, CB_GETCURSEL, 0, 0);
			value = NE::ValuePtr (new NE::BooleanValue (selected == 0 ? true : false));
		} else if (type == NUIE::ParameterType::Integer || type == NUIE::ParameterType::Float || type == NUIE::ParameterType::Double || type == NUIE::ParameterType::String) {
			wchar_t itemText[1024];
			GetDlgItemText (hwnd, controlId, itemText, 1024);
			value = NUIE::StringToParameterValue (std::wstring (itemText), type);
		} else if (type == NUIE::ParameterType::Enumeration) {
			HWND comboBoxHwnd = GetDlgItem (hwnd, controlId);
			LRESULT selected = SendMessage (comboBoxHwnd, CB_GETCURSEL, 0, 0);
			value = NE::ValuePtr (new NE::IntValue ((int) selected));
		} else {
			DBGBREAK ();
			continue;
		}

		if (paramInterface->IsValidParameterValue (paramId, value)) {
			ChangedParameter param (paramId, value);
			paramValues.push_back (param);
		} else {
			NE::ValueConstPtr oldValue = paramInterface->GetParameterValue (paramId);
			std::wstring oldValueString = NUIE::ParameterValueToString (oldValue, type);
			SetDlgItemText (hwnd, controlId, oldValueString.c_str ());
			isAllParameterValid = false;
		}
	}

	if (!isAllParameterValid) {
		paramValues.clear ();
	}
	return isAllParameterValid;
}

void ParameterDialog::ApplyParameterChanges () const
{
	for (const ChangedParameter& param : paramValues) {
		paramInterface->SetParameterValue (param.GetIndex (), param.GetValue ());
	}
}

}
