#ifndef BUTTONDECORATOR_H_INCLUDED_91AA7D58_A00A_4907_8C47_C4A409CBC08C
#define BUTTONDECORATOR_H_INCLUDED_91AA7D58_A00A_4907_8C47_C4A409CBC08C

///\class ButtonDecorator
///\brief Add nice pictures to button box buttons
///\date 12-2006
///\author Jan 'Kovis' Struhar

class QDialogButtonBox;

class ButtonDecorator
{
public:
	/// assign default icons in HolyService way
	static void assignIcons(QDialogButtonBox&);

private:
	ButtonDecorator();
	virtual ~ButtonDecorator();
};

#endif

