[%raw {|require('./ClientsFilter.less')|}];
[%raw {|require('react-dates/lib/css/_datepicker.css')|}];

open MomentRe;
open BsReactDates;

type limits = {
  min_registration_date: Js.Date.t,
  max_registration_date: Js.Date.t,
  min_points_earned: int,
  max_points_earned: int,
  min_points_spent: int,
  max_points_spent: int
};

type filter = {
  name: string,
  registration_date_from: Js.Date.t,
  registration_date_to: Js.Date.t,
  points_earned_from: int,
  points_earned_to: int,
  points_spent_from: int,
  points_spent_to: int,
};

type state = {
  filter: filter,
  focusedInput: option(Utils.focusedInput),
  expanded: bool,
};

type action =
  | NameChange(string)
  | FocusChange(option(Utils.focusedInput))
  | DatesChange(option(Moment.t), option(Moment.t))
  | EarnedPointsChange(int, int)
  | SpentPointsChange(int, int)
  | ToggleForm
  | Reset
  | Nope;

let getQueryParam = name => {
  let searchParams = [%raw "window.location.search"];
  let r = Js.Re.fromString ("(?:^\??|&)" ++ name ++ "=(.+?)(?:$|&)")
          |> Js.Re.exec(searchParams)
  
  switch r {
  | Some(result) => Js.Nullable.toOption(Js.Re.captures(result)[1])
  | None => None
  }
};

let cmpDate = (a: Js.Date.t, b: Js.Date.t) => {
  let t1 = Js.Date.getTime(a)
  let t2 = Js.Date.getTime(b)
  if (t1 > t2) { -1 } else if (t1 < t2) { 1 } else { 0 }
};

let component = ReasonReact.reducerComponent("ClientsFilter");

let make = (
  ~limits,
  ~filteredUsersCount,
  ~onSubmit=?,
  _children
  ) => {

  let getInitialFilter = () => ({
    name: switch (getQueryParam("name")) {
    | Some(value) => Js.Global.decodeURIComponent(value)
    | None => ""
    },
    registration_date_from: switch (getQueryParam("date_from")) {
    | Some(value) => {
      let date = Js.Date.fromString(Js.Global.decodeURIComponent(value));
      if (cmpDate(date, limits.min_registration_date) == -1) {
        date
      } else {
        limits.min_registration_date
      }
    }
    | None => limits.min_registration_date
    },
    registration_date_to: switch (getQueryParam("date_to")) {
    | Some(value) => let date = Js.Date.fromString(Js.Global.decodeURIComponent(value));
    if (cmpDate(date, limits.max_registration_date) == -1) {
      limits.max_registration_date
    } else {
      date
    }
    | None => limits.max_registration_date
    },
    points_earned_from: switch (getQueryParam("earned_from")) {
    | Some(value) => max(int_of_string(value), limits.min_points_earned)
    | None => limits.min_points_earned
    },
    points_earned_to: switch (getQueryParam("earned_to")) {
    | Some(value) => min(int_of_string(value), limits.max_points_earned)
    | None => limits.max_points_earned
    },
    points_spent_from: switch (getQueryParam("spent_from")) {
    | Some(value) => max(int_of_string(value), limits.min_points_spent)
    | None => limits.min_points_spent
    },
    points_spent_to: switch (getQueryParam("spent_to")) {
    | Some(value) => min(int_of_string(value), limits.max_points_spent)
    | None => limits.max_points_spent
    },
  });

  let handleSubmit = (event, {ReasonReact.state}) => {
    ReactEvent.Synthetic.preventDefault(event);
    switch onSubmit {
    | Some(fn) => fn(state.filter)
    | None => Js.log("No param passed")
    };
  };

  {
    ...component,
    initialState: () => {
      focusedInput: None,
      expanded: false,
      filter: getInitialFilter(),
    },
    didMount: self => switch onSubmit {
    | Some(fn) => fn(self.state.filter)
    | _ => ()
    },
    reducer: (action: action, state: state) => switch action {
    | NameChange(name) => ReasonReact.Update({ ...state, filter: { ...state.filter, name } })
    | DatesChange(Some(startDate), Some(endDate)) => ReasonReact.Update({...state, filter: { ...state.filter, registration_date_from: Moment.toDate(startDate), registration_date_to: Moment.toDate(endDate) }})
    | FocusChange(focusedInput) => ReasonReact.Update({...state, focusedInput})
    | EarnedPointsChange(from, to_) =>
      ReasonReact.Update({...state, filter: { ...state.filter, points_earned_from: from, points_earned_to: to_}})
    | SpentPointsChange(from, to_) =>
      ReasonReact.Update({...state, filter: { ...state.filter, points_spent_from: from, points_spent_to: to_}})
    | ToggleForm =>
      ReasonReact.Update({...state, expanded: state.expanded ? false : true})
    | Reset =>
      ReasonReact.Update({...state, filter: getInitialFilter()})
    | _ => ReasonReact.NoUpdate
    },
    render: (self) => {
      let expandedClass = switch self.state.expanded {
      | true => " _open"
      | _ => ""
      };

      <form className="filters_list" onSubmit={self.handle(handleSubmit)} action=".">
        <div className="filters_list_field">
          <input 
            onInput=(event => self.send(NameChange(ReactEvent.Form.target(event)##value)))
            className="query_input"
            placeholder={j|Поиск по имени и фамилии|j}
            value=self.state.filter.name
            type_="text" />
        </div>

        <div className="filters_list_field">
          <label className="filters_list_field_label type_date">{ReasonReact.string({j|Дата регистрации|j})}</label>
          <div className="date_picker_wrapper">
            <DateRangePicker
              startDate=momentWithDate(self.state.filter.registration_date_from)
              startDateId="regStartDate"
              endDate=momentWithDate(self.state.filter.registration_date_to)
              endDateId="regEndDate"
              focusedInput=?self.state.focusedInput
              isOutsideRange=(_day => false)
              noBorder=true
              displayFormat=Utils.DisplayFormat.Str("DD.MM.YYYY")
              onDatesChange=(v => self.send(DatesChange(v.startDate, v.endDate)))
              onFocusChange=(v => self.send(FocusChange(v)))
            />
          </div>
        </div>

        <div className=("filter_field_expand" ++ expandedClass) onClick=(_event => self.send(ToggleForm))>
          {ReasonReact.string({j|Баллы|j})}
        </div>

        <div className=("filters_list_expand" ++ expandedClass)>
          <div className="filters_list_field clearfix">
            <label className="filters_list_field_label">{ReasonReact.string({j|Кол-во заработанных баллов|j})}</label>
            <InputRange
              maxValue=limits.max_points_earned
              minValue=limits.min_points_earned
              value=InputRange.values(~min=self.state.filter.points_earned_from, ~max=self.state.filter.points_earned_to)
              step=1
              onChange=(v => self.send(EarnedPointsChange(InputRange.min(v), InputRange.max(v)))) />
            <span className="filters_field_range">
              <span>{ReasonReact.string(string_of_int(self.state.filter.points_earned_from))}</span>
              {ReasonReact.string({j| — |j})}
              <span>{ReasonReact.string(string_of_int(self.state.filter.points_earned_to))}</span>
            </span>
          </div>

          <div className="filters_list_field clearfix">
            <label className="filters_list_field_label">{ReasonReact.string({j|Кол-во потраченных баллов|j})}</label>
            <InputRange
              maxValue=limits.max_points_spent
              minValue=limits.min_points_spent
              value=InputRange.values(~min=self.state.filter.points_spent_from, ~max=self.state.filter.points_spent_to)
              step=1
              onChange=(v => self.send(SpentPointsChange(InputRange.min(v), InputRange.max(v)))) />
            <span className="filters_field_range">
              <span>{ReasonReact.string(string_of_int(self.state.filter.points_spent_from))}</span>
              {ReasonReact.string({j| — |j})}
              <span>{ReasonReact.string(string_of_int(self.state.filter.points_spent_to))}</span>
            </span>
          </div>
        </div>
        
        <div className="filters_footer clearfix">
          <div className="filters_footer_count">{ReasonReact.string(string_of_int(filteredUsersCount) ++ {j| клиентов|j})}</div>

          <button className="search_button" type_="submit">
            {ReasonReact.string({j|Показать|j})}
          </button>
          <button className="reset_button" onClick=(_event => self.send(Reset))>
            {ReasonReact.string({j|Сбросить|j})}
          </button>
        </div>
      </form>
    }
  }
};
