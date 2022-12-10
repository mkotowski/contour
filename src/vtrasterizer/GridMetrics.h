/**
 * This file is part of the "contour" project
 *   Copyright (c) 2019-2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <terminal/primitives.h>

#include <crispy/point.h>
#include <crispy/size.h>

#include <fmt/format.h>

namespace terminal::rasterizer
{

/**
 * Uniform cell margin for the grid view.
 *
 * Values are usually 0 or positive but MAY also be negative.
 */
struct CellMargin
{
    int top = 0;
    int left = 0;
    int bottom = 0;
    int right = 0;
};

/**
 * margin for the render view, so that the text isn't glued to the edge of the window/view.
 */
struct PageMargin
{
    int left;
    int top;
    int bottom;
};

/// GridMetrics contains any valuable metrics required to calculate positions on the grid.
struct GridMetrics
{
    PageSize pageSize;  // page size in column- and line count
    ImageSize cellSize; // grid cell size in pixels

    int baseline; // glyph's baseline position relative to cell bottom.

    struct
    {
        int position = 1;  // center underline position relative to cell bottom
        int thickness = 1; // underline thickness
    } underline {};

    CellMargin cellMargin {}; // TODO: implement respecting cell margins.
    PageMargin pageMargin {};

    /// Maps screen coordinates to target surface coordinates.
    ///
    /// @param col screen coordinate's column (between 0 and number of screen columns minus 1)
    /// @param line screen coordinate's line (between 0 and number of screen lines minus 1)
    ///
    /// @return 2D point into the grid cell's top left in drawing system coordinates.
    constexpr crispy::Point map(LineOffset _line, ColumnOffset _column) const noexcept
    {
        return mapTopLeft(_line, _column);
    }

    constexpr crispy::Point map(CellLocation _pos) const noexcept { return map(_pos.line, _pos.column); }

    constexpr crispy::Point mapTopLeft(CellLocation _pos) const noexcept
    {
        return mapTopLeft(_pos.line, _pos.column);
    }

    constexpr crispy::Point mapTopLeft(LineOffset _line, ColumnOffset _column) const noexcept
    {
        auto const x = pageMargin.left + *_column * cellSize.width.as<int>();
        auto const y = pageMargin.top + *_line * cellSize.height.as<int>();

        return { x, y };
    }

    constexpr crispy::Point mapBottomLeft(CellLocation _pos) const noexcept
    {
        return mapBottomLeft(_pos.line, _pos.column);
    }
    constexpr crispy::Point mapBottomLeft(LineOffset _line, ColumnOffset _column) const noexcept
    {
        return mapTopLeft(_line + 1, _column);
    }
};

} // namespace terminal::rasterizer

namespace fmt
{
template <>
struct formatter<terminal::rasterizer::GridMetrics>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }
    template <typename FormatContext>
    auto format(terminal::rasterizer::GridMetrics const& v, FormatContext& ctx)
    {
        return fmt::format_to(
            ctx.out(),
            "(pageSize={}, cellSize={}, baseline={}, underline={}@{}, margin=(left={}, bottom={}))",
            v.pageSize,
            v.cellSize,
            v.baseline,
            v.underline.position,
            v.underline.thickness,
            v.pageMargin.left,
            v.pageMargin.bottom);
    }
};

} // namespace fmt