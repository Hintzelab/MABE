# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>
#%matplotlib inline


"""
MGraph - A command-line tool for plotting CSV data with matplotlib.

Supports multiple conditions, replicates, and various statistical visualizations.
"""

from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass, field
from fnmatch import fnmatchcase
from math import ceil
from pathlib import Path
from typing import Any

# Check dependencies
from utils import pyreq
pyreq.require("matplotlib,pandas")


def create_parser() -> argparse.ArgumentParser:
    """Create and configure the argument parser."""
    parser = argparse.ArgumentParser(
        description="MGraph - Plot CSV data with statistical visualizations"
    )

    # Data file specification parameters
    parser.add_argument(
        '-path', type=str, metavar='PATH', default='',
        help='path to files - default: current directory'
    )
    parser.add_argument(
        '-conditions', type=str, metavar='CONDITION', default=[''], nargs='+',
        help='names of condition directories - default: none'
    )
    parser.add_argument(
        '-files', type=str, metavar='FILE(s)', default=['pop.csv'], nargs='+',
        help='file name(s) - default: pop.csv'
    )
    parser.add_argument(
        '-repRange', type=int, metavar=('FIRST', 'LAST'), default=[1, 0], nargs=2,
        help='replicate range - default: none'
    )
    parser.add_argument(
        '-repList', type=str, metavar='REP', default=[], nargs='+',
        help='replicate list (mutually exclusive with repRange)'
    )

    # Data filtering parameters
    parser.add_argument(
        '-data', type=str, metavar='COLUMN_NAME', default=[''], nargs='+',
        help="column names to graph (supports wildcards with '*')"
    )
    parser.add_argument(
        '-dataFromFile', type=str, metavar='FILE_NAME', default='',
        help='file to use for determining column names'
    )
    parser.add_argument(
        '-ignoreData', type=str, metavar='COLUMN_NAME', default=[''], nargs='+',
        help="column names to ignore (supports wildcards with '*')"
    )
    parser.add_argument(
        '-whereValue', type=str, default='update',
        help='column name for filtering - default: update'
    )
    parser.add_argument(
        '-whereRange', type=int, default=[], nargs='+',
        help='filter range: single value, two for range, three for range+step'
    )
    parser.add_argument(
        '-whereRangeLimitToData', action='store_true', default=False,
        help='set whereRange max based on rep with least data'
    )
    parser.add_argument(
        '-lastOnly', action='store_true', default=False,
        help='show only the last data point of all conditions'
    )

    # Data display parameters
    parser.add_argument(
        '-xAxis', type=str, metavar='COLUMN_NAME', default='update',
        help='column name for x axis - default: update'
    )
    parser.add_argument(
        '-dataIndex', type=str, metavar='COLUMN_NAME', default='update',
        help='column name for averaging index - default: update'
    )
    parser.add_argument(
        '-yRange', type=float, default=[], nargs='+',
        help='y axis range (expects 2 values)'
    )
    parser.add_argument(
        '-xRange', type=float, default=[], nargs='+',
        help='x axis range (expects 2 values)'
    )
    parser.add_argument(
        '-pltWhat', type=str, metavar='{ave,std,sem,95conf,99conf,reps}',
        choices=('ave', 'std', 'sem', '95conf', '99conf', 'reps'),
        default=['ave', '95conf'], nargs='+',
        help='what to plot - default: ave 95conf'
    )
    parser.add_argument(
        '-integrate', type=str, default=[], metavar='_DATA', nargs='+',
        help='integrate associated data with _AVE data of same name'
    )
    parser.add_argument(
        '-combineConditions', action='store_true', default=False,
        help='combine data from files with same name across conditions'
    )
    parser.add_argument(
        '-combineData', action='store_true', default=False,
        help='combine multiple data lines into one plot'
    )

    # Plot parameters
    parser.add_argument(
        '-title', type=str, default='NONE',
        help='title of image - default: auto-generated'
    )
    parser.add_argument(
        '-conditionNames', type=str, metavar='CONDITION_NAME', default=[''], nargs='+',
        help='display names for conditions'
    )
    parser.add_argument(
        '-imageSize', type=float, default=[10, 10], nargs=2,
        help='size of image - default: 10 10'
    )
    parser.add_argument(
        '-pltStyle', type=str, choices=('line', 'point', 'randomLine', 'randomPoint'),
        default='line', help='plot style - default: line'
    )
    parser.add_argument(
        '-errorStyle', type=str, choices=('region', 'bar', 'barX', 'barXY'),
        default='region', help='error display style - default: region'
    )
    parser.add_argument(
        '-numCol', type=int, metavar='#', default=3,
        help='number of columns in multi-plot - default: 3'
    )
    parser.add_argument(
        '-legendLocation', type=str,
        choices=('ur', 'ul', 'lr', 'll', 'cr', 'cl', 'lc', 'uc', 'c', 'off'),
        default='lr', help='legend placement - default: lr (lower right)'
    )
    parser.add_argument(
        '-legendLineWeight', type=int, default=-1,
        help='line thickness in legend - default: same as lineWeight'
    )
    parser.add_argument(
        '-lineWeight', type=int, default=1,
        help='line thickness in plots - default: 1'
    )
    parser.add_argument(
        '-grid', action='store_true', default=False,
        help='display grid on plots'
    )
    parser.add_argument(
        '-fontSizeMajor', type=int, default=15,
        help='main title font size - default: 15'
    )
    parser.add_argument(
        '-fontSizeMinor', type=int, default=10,
        help='subplot title font size - default: 10'
    )
    parser.add_argument(
        '-fontSizeTicks', type=int, default=8,
        help='axis tick font size - default: 8'
    )
    parser.add_argument(
        '-fontSizeLegend', type=int, default=8,
        help='legend font size - default: 8'
    )

    # Utility parameters
    parser.add_argument(
        '-showDataNames', action='store_true', default=False,
        help='print column names from first file and exit'
    )
    parser.add_argument(
        '-verbose', action='store_true', default=False,
        help='provide detailed output while running'
    )
    parser.add_argument(
        '-save', type=str, choices=('pdf', 'png', ''), default='',
        help='save format (pdf or png) - default: display only'
    )
    parser.add_argument(
        '-saveName', type=str, default='',
        help='custom filename when saving single file'
    )

    return parser


@dataclass
class PlotConfig:
    """Configuration for plot styling and layout."""
    plt_what: list[str] = field(default_factory=lambda: ['ave', '95conf'])
    plt_style: str = 'line'
    error_style: str = 'region'
    columns: int = 3
    legend_location: str = 'lower right'
    x_range: list[float] = field(default_factory=list)
    y_range: list[float] = field(default_factory=list)
    integrate_names: list[str] = field(default_factory=list)
    image_size: tuple[float, float] = (10, 10)
    line_weight: int = 1
    legend_line_weight: int = -1
    show_grid: bool = False
    last_only: bool = False
    combine_data: bool = False
    font_size_major: int = 15
    font_size_minor: int = 10
    font_size_ticks: int = 8
    font_size_legend: int = 8
    title: str = ''
    custom_title: str | None = None


def validate_args(args: argparse.Namespace) -> None:
    """Validate command line arguments for conflicts."""
    if args.repRange != [1, 0] and args.repList:
        print('Error: -repRange and -repList are mutually exclusive')
        sys.exit(1)

    if args.saveName and args.save == 'png' and len(args.files) > 1:
        print('\nError: -saveName provided but multiple image files will be created.\n'
              '  Options:\n'
              '  - Save as pdf for combined output\n'
              '  - Run mGraph for each input file\n'
              '  - Remove -saveName')
        sys.exit(1)


def isolate_condition(df: 'pandas.DataFrame', condition: str) -> 'pandas.DataFrame':
    """Filter dataframe to rows matching the given condition."""
    return df.loc[df['con'] == condition]


def add_error_bars(
    plt: Any,
    error_style: str,
    x_values: Any,
    avg_line: Any,
    y_error: Any,
    color: tuple,
    x_error: Any = None
) -> None:
    """Add error visualization to plot based on style."""
    if error_style == 'bar':
        plt.errorbar(x_values, avg_line, yerr=y_error, color=color, alpha=0.5, fmt='.')
    elif error_style == 'barX':
        if x_error is not None:
            plt.errorbar(x_values, avg_line, xerr=x_error, color=color, alpha=0.5, fmt='.')
    elif error_style == 'barXY':
        if x_error is not None:
            plt.errorbar(x_values, avg_line, xerr=x_error, yerr=y_error, color=color, alpha=0.5, fmt='.')
    elif error_style == 'region':
        plt.fill_between(x_values, avg_line - y_error, avg_line + y_error, color=color, alpha=0.15)


def get_style_config(plt_style: str) -> tuple[list[str], str]:
    """Get style list and base style for plotting."""
    style_configs = {
        'line': (['-'], '-'),
        'point': (['o', '*', 's', 'D', '^', '.'], 'o'),
        'randomLine': (['-^', '-.', '-o', '-*', '-s', '-D'], '-'),
        'randomPoint': (['^', '.', 'o', '*', 's', 'D'], 'o'),
    }
    return style_configs.get(plt_style, (['-'], '-'))


def setup_subplot(
    ax: Any,
    name: str,
    config: PlotConfig,
    conditions_list: list[str],
    x_coord_name: str
) -> None:
    """Configure subplot appearance."""
    import matplotlib.pyplot as plt

    if config.show_grid:
        plt.grid(visible=True, which='major', color=(0, 0, 0), linestyle='-', alpha=0.25)

    if config.last_only:
        plt.xticks(range(len(conditions_list)), conditions_list, rotation=45, ha='right')
    else:
        plt.ticklabel_format(useOffset=False, style='plain')

    plt.tick_params(labelsize=config.font_size_ticks)

    if len(config.x_range) == 2:
        plt.xlim(config.x_range[0], config.x_range[1])

    if len(config.y_range) == 2:
        plt.ylim(config.y_range[0], config.y_range[1])


def plot_reps(
    plt: Any,
    data: 'pandas.DataFrame',
    reps: list[str],
    condition: str,
    x_coord_name: str,
    name: str,
    style: str,
    color: tuple,
    label: str
) -> None:
    """Plot individual replicate lines."""
    first_rep = True
    for rep in reps:
        mask = (data["repName"] == rep) & (data["con"] == condition)
        rep_data = data.loc[mask]
        rep_label = f"{label}_rep" if first_rep else '_nolegend_'
        plt.plot(
            rep_data[x_coord_name],
            rep_data[name],
            style,
            alpha=0.25,
            color=color,
            label=rep_label
        )
        first_rep = False


def plot_statistics(
    plt: Any,
    config: PlotConfig,
    x_values: Any,
    df_mean: 'pandas.DataFrame',
    df_std: 'pandas.DataFrame | None',
    df_sem: 'pandas.DataFrame | None',
    name: str,
    all_names: list[str],
    condition: str,
    style: str,
    color: tuple,
    label: str,
    verbose: bool = False
) -> None:
    """Plot statistical lines (average, std, sem, confidence intervals)."""
    avg_line = df_mean.loc[:, name]

    # Plot integrated variance data if requested
    for integrate_name in config.integrate_names:
        var_name = name[:-4] + integrate_name
        if var_name in all_names and var_name in df_mean.columns:
            if verbose:
                print(f'     {var_name} found, adding to plot for data: {name} condition: {condition}', flush=True)
            error_y = df_mean.loc[:, var_name]
            plt.fill_between(x_values, avg_line - error_y, avg_line + error_y, color=color, alpha=0.15)

    if 'std' in config.plt_what and df_std is not None:
        error_y = df_std.loc[:, name]
        add_error_bars(plt, config.error_style, x_values, avg_line, error_y, color)

    if 'ave' in config.plt_what:
        plt.plot(x_values, avg_line, style, markersize=10, color=color,
                 linewidth=config.line_weight, label=label)

    if df_sem is not None:
        if 'sem' in config.plt_what:
            error_y = df_sem.loc[:, name]
            add_error_bars(plt, config.error_style, x_values, avg_line, error_y, color)

        if '95conf' in config.plt_what:
            error_y = df_sem.loc[:, name].multiply(1.96)
            add_error_bars(plt, config.error_style, x_values, avg_line, error_y, color)

        if '99conf' in config.plt_what:
            error_y = df_sem.loc[:, name].multiply(2.58)
            add_error_bars(plt, config.error_style, x_values, avg_line, error_y, color)


def add_legend(
    plt: Any,
    config: PlotConfig,
    conditions_list: list[str],
    x_coord_name: str
) -> None:
    """Add legend to current plot if needed."""
    if config.legend_location:
        if config.last_only:
            plt.xlabel('Conditions', fontsize=config.font_size_minor)
        else:
            plt.xlabel(x_coord_name, fontsize=config.font_size_minor)

        leg = plt.legend(fontsize=config.font_size_legend, loc=config.legend_location)
        if config.legend_line_weight > 0:
            for legobj in leg.legend_handles:
                legobj.set_linewidth(config.legend_line_weight)


def multi_plot(
    data: 'pandas.DataFrame',
    names_list: list[str],
    conditions_list: list[str],
    data_index: str,
    reps: list[str],
    x_coord_name: str,
    config: PlotConfig,
    verbose: bool = False
) -> Any:
    """
    Create multi-panel plot with statistical visualizations.

    Args:
        data: DataFrame containing all data
        names_list: Column names to plot
        conditions_list: List of condition names
        data_index: Column name for grouping/averaging
        reps: List of replicate identifiers
        x_coord_name: Column name for x-axis values
        config: Plot configuration options
        verbose: Enable verbose output

    Returns:
        matplotlib figure object
    """
    import matplotlib.cm as cm
    import matplotlib.pyplot as plt

    color_map = cm.gist_rainbow
    style_list, base_style = get_style_config(config.plt_style)
    current_style = base_style
    current_color = (0, 0, 0)

    # Ensure enough styles for all combinations
    num_combinations = len(conditions_list) * len(names_list)
    while len(style_list) < num_combinations:
        style_list = style_list + style_list

    fig = plt.figure(figsize=config.image_size)
    fig.subplots_adjust(hspace=0.35)

    # Remove axis columns from plot list
    names_list = [n for n in names_list if n not in (x_coord_name, data_index)]
    if verbose:
        if x_coord_name in names_list:
            print(f'Removing xAxis column: {x_coord_name} from plot list.', flush=True)
        if data_index in names_list:
            print(f'Removing dataIndex column: {data_index} from plot list.', flush=True)

    # Keep original list for integrate lookups
    all_names_list = names_list.copy()

    # Remove integrate columns from main plot list
    for integrate_name in config.integrate_names:
        names_list = [x for x in names_list if integrate_name not in x]

    # Adjust column count based on number of plots
    columns = config.columns
    if len(names_list) == 1:
        columns = 1
    elif len(names_list) == 2 and columns > 2:
        columns = 2

    rows = ceil(len(names_list) / columns)

    # Set title
    title = config.title
    if config.last_only:
        title += '    x axis = conditions'
    else:
        title += f'    x axis = {x_coord_name}'

    if config.custom_title:
        title = config.custom_title

    plt.suptitle(title, fontsize=config.font_size_major, fontweight='bold')

    for cond_idx, condition in enumerate(conditions_list):
        df_cond = isolate_condition(data, condition).groupby(data_index)

        # Compute statistics
        df_mean = df_cond.mean(numeric_only=True)
        df_std = df_cond.std(numeric_only=True) if 'std' in config.plt_what else None
        df_sem = None
        if any(x in config.plt_what for x in ['sem', '95conf', '99conf']):
            df_sem = df_cond.sem(numeric_only=True)

        # Get x-axis values
        x_values = (
            isolate_condition(data, condition)
            .pivot(index=data_index, columns='repName', values=x_coord_name)
            .mean(axis=1)
        )

        for name_idx, name in enumerate(names_list):
            # Determine label and styling
            if not config.combine_data:
                label = condition
                ax = plt.subplot(rows, columns, name_idx + 1)
                plt.title(name, fontsize=config.font_size_minor)
                ax.title.set_position([0.5, 1])

                if len(conditions_list) > 1:
                    current_style = style_list[cond_idx]
                    current_color = color_map(cond_idx / len(conditions_list))
            else:
                combo_idx = cond_idx + (name_idx * len(conditions_list))
                current_style = style_list[combo_idx]
                total = len(conditions_list) + len(names_list)
                current_color = color_map(combo_idx / total)

                if len(conditions_list) == 1:
                    label = name
                else:
                    label = f'{condition} {name}'

            if config.show_grid:
                plt.grid(visible=True, which='major', color=(0, 0, 0), linestyle='-', alpha=0.25)

            # Check if column is valid
            if name not in df_mean.columns:
                if name == data_index:
                    print(f'Warning: Cannot plot {name} (it is the data index).', flush=True)
                    if not config.combine_data:
                        plt.title(f'{name}\n(invalid: dataIndex)', fontsize=config.font_size_minor)
                else:
                    print(f'Warning: {name} is non-numeric and cannot be plotted.', flush=True)
                    if not config.combine_data:
                        plt.title(f'{name} (INVALID DATA FORMAT)', fontsize=config.font_size_minor)
                continue

            if config.last_only:
                # Bar plot for last values only
                quantity = df_mean.loc[:, name].tail(1).iloc[0]
                if 'std' in config.plt_what and df_std is not None:
                    quantity_err = df_std.loc[:, name].tail(1)
                    plt.bar([cond_idx], [quantity], yerr=quantity_err)
                else:
                    plt.bar([cond_idx], [quantity])
            else:
                # Line plots
                if 'reps' in config.plt_what:
                    plot_reps(plt, data, reps, condition, x_coord_name, name,
                              current_style, current_color, label)

                if any(x in config.plt_what for x in ['ave', 'std', 'sem', '95conf', '99conf']):
                    plot_statistics(
                        plt, config, x_values, df_mean, df_std, df_sem,
                        name, all_names_list, condition, current_style,
                        current_color, label, verbose
                    )

            # Add legend if multiple conditions or combined data
            if (len(conditions_list) > 1 or config.combine_data) and config.legend_location:
                add_legend(plt, config, conditions_list, x_coord_name)

            setup_subplot(plt.gca(), name, config, conditions_list, x_coord_name)

    return fig


def get_rep_list(args: argparse.Namespace) -> list[str]:
    """Build list of replicate directory names."""
    if args.repList:
        reps = args.repList
    elif args.repRange != [1, 0]:
        reps = list(range(args.repRange[0], args.repRange[1] + 1))
    else:
        return ['']

    return [f'{r}/' for r in reps]


def get_condition_names(args: argparse.Namespace) -> tuple[list[str], list[str]]:
    """Get folder names and display names for conditions."""
    folder_names = args.conditions.copy()

    if folder_names != ['']:
        folder_names = [f'{name}/' if not name.endswith('/') else name for name in folder_names]

    if args.conditionNames == ['']:
        user_names = [name.rstrip('/') for name in folder_names]
    else:
        user_names = args.conditionNames
        if len(user_names) != len(folder_names):
            print('Error: -conditions and -conditionNames must have the same number of arguments')
            sys.exit(1)

    return folder_names, user_names


def get_data_names(
    args: argparse.Namespace,
    condition_folder_names: list[str],
    replicates: list[str]
) -> list[str]:
    """Get list of column names to plot from exemplar file."""
    data_from_file = args.dataFromFile if args.dataFromFile else args.files[0]
    base_path = Path(args.path) if args.path else Path('.')
    exemplar_path = base_path / condition_folder_names[0] / replicates[0] / data_from_file

    if args.verbose:
        print(f'Getting column names from {exemplar_path}', flush=True)

    try:
        with open(exemplar_path, 'r') as f:
            names_from_file = f.readline().strip().split(',')
    except FileNotFoundError:
        print(f'Error: Could not find file {exemplar_path}')
        sys.exit(1)

    if args.showDataNames:
        print('Column names:')
        print(','.join(names_from_file))
        sys.exit(0)

    # Build names list from user specification or file
    names_list = []
    if args.data != ['']:
        for pattern in args.data:
            if '*' in pattern or '[' in pattern or ']' in pattern:
                if args.verbose:
                    print(f'Found wildcard pattern: {pattern}', flush=True)
                for name in names_from_file:
                    if fnmatchcase(name, pattern):
                        if args.verbose:
                            print(f'   Matched: {name}', flush=True)
                        names_list.append(name)
            else:
                names_list.append(pattern)
    else:
        names_list = names_from_file.copy()

    # Remove x-axis column
    if args.xAxis in names_list:
        names_list.remove(args.xAxis)

    # Apply ignore patterns
    for pattern in args.ignoreData:
        if '*' in pattern or '[' in pattern or ']' in pattern:
            if args.verbose:
                print(f'Ignoring pattern: {pattern}', flush=True)
            names_list = [n for n in names_list if not fnmatchcase(n, pattern)]

    return names_list


def find_alternate_data_names(search_from: list[str], match_to: list[str], verbose: bool = False) -> list[str]:
    """Find matching column names, with fallback for _AVE suffix."""
    alternate_names = []
    for name in match_to:
        if name in search_from:
            alternate_names.append(name)
        else:
            if verbose:
                print(f"  Can't find: '{name}'", flush=True)
            if name.endswith('_AVE'):
                short_name = name[:-4]
                if short_name in search_from:
                    if verbose:
                        print(f"         Found: '{short_name}'", flush=True)
                    alternate_names.append(short_name)
    return alternate_names


def load_data(
    args: argparse.Namespace,
    condition_folder_names: list[str],
    condition_user_names: list[str],
    replicates: list[str],
    file_names: list[str],
    data_names: list[str]
) -> tuple[dict[str, 'pandas.DataFrame'], int | str]:
    """Load and combine data from all files, conditions, and replicates."""
    import pandas
    from pandas import concat, read_csv

    df_dict: dict[str, list] = {}
    update_min: int | str = 'undefined'
    base_path = Path(args.path) if args.path else Path('.')
    data_from_file = args.dataFromFile if args.dataFromFile else args.files[0]

    for file_name in file_names:
        df_dict[file_name] = []
        alt_names: list[str] = []

        for folder, user_name in zip(condition_folder_names, condition_user_names):
            for rep in replicates:
                file_path = base_path / folder / rep / file_name

                if args.verbose:
                    print(f'Loading: {file_path}', flush=True)

                try:
                    df_all = read_csv(file_path)
                except FileNotFoundError:
                    print(f'Error: File not found: {file_path}')
                    sys.exit(1)

                # Track minimum data extent
                last_x = df_all[args.xAxis].iat[-1]
                if update_min == 'undefined' or last_x < update_min:
                    update_min = last_x
                    if args.verbose:
                        print(f'{user_name} {rep} has data until: {last_x} (new minimum)', flush=True)

                # Determine columns to keep
                if args.xAxis == args.dataIndex:
                    extra_columns = [args.dataIndex]
                else:
                    extra_columns = [args.xAxis, args.dataIndex]

                if file_name == data_from_file:
                    columns_to_keep = list(set(data_names + extra_columns))
                else:
                    if not alt_names:
                        alt_names = find_alternate_data_names(
                            list(df_all.columns), data_names, args.verbose
                        )
                    columns_to_keep = list(set(alt_names + extra_columns))

                # Filter columns (only keep those that exist)
                existing_columns = [c for c in columns_to_keep if c in df_all.columns]
                df_keep = df_all[existing_columns].copy()

                df_keep['repName'] = rep
                df_keep['con'] = user_name
                df_dict[file_name].append(df_keep)

    return {f: concat(dfs, ignore_index=True) for f, dfs in df_dict.items()}, update_min


def apply_where_filter(
    data_frames: dict[str, 'pandas.DataFrame'],
    where_value: str,
    where_range: list[int]
) -> dict[str, 'pandas.DataFrame']:
    """Apply range filtering to dataframes."""
    result = {}
    for name, df in data_frames.items():
        if len(where_range) == 1:
            df = df.loc[df[where_value] == where_range[0]]
        elif len(where_range) == 2:
            df = df.loc[(df[where_value] >= where_range[0]) & (df[where_value] <= where_range[1])]
        elif len(where_range) >= 3:
            crop_range = list(range(where_range[0], where_range[1] + 1, where_range[2]))
            df = df.loc[df[where_value].isin(crop_range)]
        result[name] = df
    return result


def get_legend_location(abbrev: str) -> str:
    """Convert abbreviated legend location to matplotlib format."""
    mapping = {
        'ur': 'upper right', 'ul': 'upper left',
        'lr': 'lower right', 'll': 'lower left',
        'cr': 'center right', 'cl': 'center left',
        'lc': 'lower center', 'uc': 'upper center',
        'c': 'center', 'off': ''
    }
    return mapping.get(abbrev, 'lower right')


def save_plots(
    all_graphs: dict[str, Any],
    save_format: str,
    save_name: str,
    title: str
) -> None:
    """Save plots to file(s)."""
    from matplotlib.backends.backend_pdf import PdfPages

    if save_format == 'png':
        for graph_name, fig in all_graphs.items():
            if save_name:
                filename = f'{save_name}.png'
            elif graph_name.endswith('.csv'):
                filename = f'{title}_MGraph_{graph_name[:-4].replace("/", "_")}.png'
            elif title != 'NONE':
                filename = f'{title}_MGraph_{graph_name.replace("/", "_")}.png'
            else:
                filename = f'MGraph_{graph_name.replace("/", "_")}.png'
            fig.savefig(filename, dpi=100)

    elif save_format == 'pdf':
        filename = f'{save_name}.pdf' if save_name else f'{title}_MGraph.pdf'
        with PdfPages(filename) as pp:
            for fig in all_graphs.values():
                pp.savefig(fig)


def main(args: argparse.Namespace) -> None:
    """Main entry point for MGraph."""
    import matplotlib.pyplot as plt

    plt.rcParams['figure.figsize'] = (6, 6)

    # Normalize path
    if args.path and not args.path.endswith('/'):
        args.path += '/'

    # Get data specifications
    reps = get_rep_list(args)
    folder_names, user_names = get_condition_names(args)
    data_column_names = get_data_names(args, folder_names, reps)

    # Load data
    data_frames, update_min = load_data(
        args, folder_names, user_names, reps, args.files, data_column_names
    )

    # Handle whereRange limits
    if not args.whereRange and args.whereRangeLimitToData:
        args.whereRange = [0, update_min]
    elif len(args.whereRange) >= 2 and args.whereRangeLimitToData:
        args.whereRange[1] = update_min

    # Apply filtering
    if args.whereRange:
        data_frames = apply_where_filter(data_frames, args.whereValue, args.whereRange)

    # Create plot configuration
    config = PlotConfig(
        plt_what=args.pltWhat,
        plt_style=args.pltStyle,
        error_style=args.errorStyle,
        columns=args.numCol,
        legend_location=get_legend_location(args.legendLocation),
        x_range=args.xRange,
        y_range=args.yRange,
        integrate_names=args.integrate,
        image_size=tuple(args.imageSize),
        line_weight=args.lineWeight,
        legend_line_weight=args.legendLineWeight,
        show_grid=args.grid,
        last_only=args.lastOnly,
        combine_data=args.combineData,
        font_size_major=args.fontSizeMajor,
        font_size_minor=args.fontSizeMinor,
        font_size_ticks=args.fontSizeTicks,
        font_size_legend=args.fontSizeLegend,
        custom_title=args.title if args.title != 'NONE' else None,
    )

    # Generate plots
    all_graphs = {}

    if args.combineConditions:
        for file_name in args.files:
            if args.verbose:
                print(f'Generating plot for: {file_name}', flush=True)

            names_list = find_alternate_data_names(
                list(isolate_condition(data_frames[file_name], user_names[0]).columns),
                data_column_names,
                args.verbose
            )

            config.title = file_name
            all_graphs[file_name] = multi_plot(
                data=data_frames[file_name],
                names_list=names_list,
                conditions_list=user_names,
                data_index=args.dataIndex,
                reps=reps,
                x_coord_name=args.xAxis,
                config=config,
                verbose=args.verbose
            )
    else:
        for condition in user_names:
            for file_name in args.files:
                graph_key = f'{condition}__{file_name}'
                if args.verbose:
                    print(f'Generating plot for: {graph_key}', flush=True)

                names_list = find_alternate_data_names(
                    list(isolate_condition(data_frames[file_name], user_names[0]).columns),
                    data_column_names,
                    args.verbose
                )

                config.title = graph_key
                all_graphs[graph_key] = multi_plot(
                    data=data_frames[file_name],
                    names_list=names_list,
                    conditions_list=[condition],
                    data_index=args.dataIndex,
                    reps=reps,
                    x_coord_name=args.xAxis,
                    config=config,
                    verbose=args.verbose
                )

    # Output results
    if args.save:
        save_plots(all_graphs, args.save, args.saveName, args.title)
    else:
        plt.show()


if __name__ == '__main__':
    parser = create_parser()
    args = parser.parse_args()
    validate_args(args)

    if args.save:
        from matplotlib import use
        use('Agg')

    main(args)

