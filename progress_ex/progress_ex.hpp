#include <nana/gui/drawing.hpp>
#include <nana/gui/widgets/progress.hpp>

#include <array>

namespace nana
{
	class progress_ex : public progress
	{
	public:

		enum class text_modes { caption, percentage, value };
		enum class color_presets { nana, blue, green, orange, violet, brown };

		bool create(window parent, bool visible = true)
		{
			return create(parent, rectangle{}, visible);
		}

		bool create(window parent, const rectangle &r = {}, bool visible = true)
		{
			if(progress::create(parent, r, visible))
			{
				typeface({typeface().name(), typeface().size(), paint::font::font_style{800}});
				presets[color_presets::nana] = {
					scheme().gradient_fgcolor, scheme().foreground, scheme().background, scheme().gradient_bgcolor
				};

				drawing(*this).draw([this](paint::graphics &graph)
				{
					switch(tmode)
					{
					case text_modes::caption:
						if(!text.empty())
							render_text(graph, text);
						break;

					case text_modes::value:
						if(!unknown())
							render_text(graph, std::to_string(value()) + " / " + std::to_string(amount()));
						break;

					case text_modes::percentage:
						if(!unknown())
							render_text(graph, std::to_string(value()/(amount()/100)) + '%');
						break;
					}
				});

				return true;
			}
			return false;
		}

		progress_ex() : progress() {}
		progress_ex(window parent, bool visible) { create(parent, visible); }
		progress_ex(window parent, const rectangle &r = {}, bool visible = true) { create(parent, r, visible); }

		widget& caption(std::string utf8)
		{
			text = utf8;
			API::refresh_window(*this);
			return progress::caption(utf8);
		}

		widget& caption(std::wstring wstr)
		{
			text = charset{wstr}.to_bytes(unicode::utf8);
			API::refresh_window(*this);
			return progress::caption(wstr);
		}

		void text_mode(text_modes m) { tmode = m; API::refresh_window(*this); }
		text_modes text_mode() const noexcept { return tmode; }

		// this color is used if text contrast is disabled
		void text_color(color clr) { clr_normal = clr;  API::refresh_window(*this); }
		color text_color() { return clr_normal; }
		
		void text_contrast_colors(color full, color empty) { clr_left = full; clr_right = empty; API::refresh_window(*this); }
		std::pair<color, color> text_contrast_colors() { return std::make_pair(clr_left, clr_right); }

		void text_contrast(bool enable) { contrast = enable; API::refresh_window(*this); }
		bool text_contrast() { return contrast; }

		// shade: [-1.0] to [1.0] (negative = darker, zero = default, positive = lighter)
		void color_preset(color_presets preset, double shade = 0)
		{
			auto colors = presets[preset];
			auto blend_color = shade < 0 ? colors::black : colors::white;
			shade = abs(shade);
			scheme().gradient_fgcolor = colors[0].blend(blend_color, shade);
			scheme().foreground = colors[1].blend(blend_color, shade);
			scheme().background = colors[2];
			scheme().gradient_bgcolor = colors[3];
		}

	private:

		std::string text;
		text_modes tmode{text_modes::caption};
		color clr_left{"#ffffff"}, clr_right{"#333333"}, clr_normal{clr_right};
		bool contrast{true};

		std::unordered_map<color_presets, std::array<color, 4>> presets
		{
			//                           __/top fg\__      __/bot fg\__      __/top bg\__      __/bot bg\__
			{ color_presets::blue,   { color{"#bbd7df"}, color{"#407f91"}, color{"#d8d6d3"}, color{"#fbf9f6"} } },
			{ color_presets::orange, { color{"#F2C9B5"}, color{"#CF6430"}, color{"#d8d6d3"}, color{"#fbf9f6"} } },
			{ color_presets::green,  { color{"#AFD5AA"}, color{"#508D47"}, color{"#d8d6d3"}, color{"#fbf9f6"} } },
			{ color_presets::violet, { color{"#ccc8ce"}, color{"#75617E"}, color{"#d8d6d3"}, color{"#fbf9f6"} } },
			{ color_presets::brown,  { color{"#DFC9B3"}, color{"#8B633A"}, color{"#d8d6d3"}, color{"#fbf9f6"} } }
		};

		void render_text(paint::graphics &graph, const std::string &text)
		{
			auto tsize{graph.text_extent_size(text)};
			auto gsize{graph.size()};
			nana::point pos{0,0};
			if(tsize.width < gsize.width)
				pos.x = gsize.width/2 - tsize.width/2;
			if(tsize.height < gsize.height)
				pos.y = gsize.height/2 - tsize.height/2;

			if(contrast && !unknown())
			{
				double unit = static_cast<double>(amount()) / static_cast<double>(graph.width()-2);
				unsigned pixels = static_cast<double>(value()) / unit;

				if(pixels < pos.x)
					graph.string(pos, text, clr_right);
				else if(pixels > pos.x + tsize.width)
					graph.string(pos, text, clr_left);
				else
				{
					paint::graphics left{graph.size()}, right{graph.size()};
					graph.paste(left, 0, 0);
					graph.paste(right, 0, 0);

					left.typeface(typeface());
					right.typeface(typeface());
					left.string(pos, text, clr_left);
					right.string(pos, text, clr_right);

					graph.bitblt({0, 0, pixels+1, graph.height()}, left);
					graph.bitblt({static_cast<int>(pixels)+1, 0, graph.width()-2-pixels, graph.height()},
						right, {static_cast<int>(pixels)+1, 0});
				}
			}
			else graph.string(pos, text, clr_normal);
		}
	};
}