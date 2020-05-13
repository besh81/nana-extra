/*
 *		nana::pgitems Implementation
 *
 *      part of Nana Creator (https://github.com/besh81/nana-creator)
 *      Author: besh81
 */

#ifndef NANA_CREATOR_PGITEMS_H
#define NANA_CREATOR_PGITEMS_H

#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include "propertygrid.h"


#define PG_BORDER_X		1
#define PG_BORDER_Y		2


namespace nana
{
	using pgitem = drawerbase::propertygrid::pgitem;


	/// class pg_string
	class pg_string
		: public pgitem
	{
	public:
		pg_string() = default;

		pg_string(const std::string& label, const std::string& value)
			: pgitem(label, value)
		{}

		virtual void value(const std::string& value) override;

		virtual void enabled(bool state) override;

		virtual void set_accept(std::function<bool(wchar_t)> f);

		virtual void editable(bool editable);
		virtual bool editable();

		virtual void tooltip(const std::string& help_text) override;	///< Provide pop-up help string

	protected:
		virtual void create(window wd) override;

		virtual void draw_value(paint::graphics* graph, rectangle rect, const int txtoff, color bgcolor, color fgcolor) const override;

		mutable nana::textbox	txt_;
	};


	/// class pg_string_int
	class pg_string_int
		: public pg_string
	{
	public:
		pg_string_int() = default;

		pg_string_int(const std::string& label, const std::string& value)
			: pg_string(label, value)
		{}

		virtual void value(int value);
		virtual int to_int() const;

		virtual void range(int min, int max);

	protected:
		bool validate_user_input(int & value);

		virtual void create(window wd) override;

		bool	range_{ false };
		int		min_;
		int		max_;
	};


	/// class pg_string_uint
	class pg_string_uint
		: public pg_string
	{
	public:
		pg_string_uint() = default;

		pg_string_uint(const std::string& label, const std::string& value)
			: pg_string(label, value)
		{}

		virtual void value(unsigned value);
		virtual unsigned to_uint() const;

		virtual void range(unsigned min, unsigned max);

	protected:
		bool validate_user_input(unsigned & value);

		virtual void create(window wd) override;

		bool		range_{ false };
		unsigned	min_;
		unsigned	max_;
	};


	/// class pg_string_button
	class pg_string_button
		: public pgitem
	{
	public:
		pg_string_button() = default;

		pg_string_button(const std::string& label, const std::string& value, const std::string& button_label = "...")
			: pgitem(label, value), btn_label_(button_label)
		{}

		virtual void value(const std::string& value) override;

		virtual void enabled(bool state) override;

		virtual void set_accept(std::function<bool(wchar_t)> f);
		virtual void set_button_click(std::function<void(const nana::arg_click&)> f);

		virtual void editable(bool editable);
		virtual bool editable() const;

		virtual void tooltip(const std::string& help_text) override;	///< Provide pop-up help string

	protected:
		virtual void create(window wd) override;

		virtual void draw_value(paint::graphics* graph, rectangle rect, const int txtoff, color bgcolor, color fgcolor) const override;

		std::string				btn_label_;
		mutable nana::textbox	txt_;
		mutable nana::button	btn_;
	};


	/// class pg_choice
	class pg_choice
		: public pgitem
	{
	public:
		pg_choice() = default;

		pg_choice(const std::string& label)
			: pgitem(label)
		{}

		virtual void value(const std::string& value) override;

		virtual void enabled(bool state) override;

		virtual void option(std::size_t value);
		virtual std::size_t option() const;

		virtual void push_back(const std::string& option);
		virtual void set(const std::vector<std::string>& vs);	///< Set options to be chosen from

		virtual void tooltip(const std::string& help_text) override;	///< Provide pop-up help string

	protected:
		virtual void create(window wd) override;

		virtual void draw_value(paint::graphics* graph, rectangle rect, const int txtoff, color bgcolor, color fgcolor) const override;

		mutable nana::combox	cmb_;
	};


	/// class pg_check
	class pg_check
		: public pgitem
	{
	public:
		pg_check() = default;

		pg_check(const std::string& label, bool value)
			: pgitem(label, value ? "true" : "false")
		{}

		virtual void value(const std::string& value) override;

		virtual void enabled(bool state) override;

		virtual void check(bool value)
		{
			pg_check::value(value ? "true" : "false");
		}
		virtual bool checked() const
		{
			return pgitem::value() == "true" ? true : false;
		}

	virtual void tooltip(const std::string& help_text) override;	///< Provide pop-up help string

	protected:
		virtual void create(window wd) override;

		virtual void draw_value(paint::graphics* graph, rectangle rect, const int txtoff, color bgcolor, color fgcolor) const override;

		mutable nana::checkbox	chk_;
	};


	/// class pg_color
	class pg_color
		: public pgitem
	{
	public:
		pg_color() = default;

		pg_color(const std::string& label, const std::string& value)
			: pgitem(label, value)
		{}

		virtual void value(const std::string& value) override;

		virtual void enabled(bool state) override;

		virtual void value(nana::color value);
		virtual nana::color to_color() const
		{
			return color_;
		}

		virtual unsigned size() const override
		{
			return expand_ ? 2 * size_ : size_;
		}

	protected:
		virtual void create(window wd) override;

		virtual void draw(paint::graphics* graph, rectangle area, unsigned labelw, unsigned  valuew, unsigned  iboxw, const int txtoff, color bgcolor, color fgcolor) const override;

		mutable nana::textbox	colorbox_;
		mutable nana::textbox	rgb_[3];
		bool		expand_{ false };

		nana::color	color_;
	};

}//end namespace nana

#endif //NANA_CREATOR_PGITEMS_H
