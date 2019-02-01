[%raw {|require('./ClientsList.less')|}]

type user = {
  id: string,
  name: string,
  points_earned: int,
  points_spent: int,
  registration_date: Js.Date.t,
};

type order_field =
  | RegistrationDate
  | Name
  | Points;

type order_by =
  | Asc
  | Desc;

type action =
  | Sort(order_field, order_by)
  | PageChange(int);

type state = {
  users: array(user),
  currentPage: int,
  orderField: order_field,
  orderBy: order_by,
};

let cmp = (a, b) => if (a > b) { -1 } else if (a < b) { 1 } else { 0 };

let sort = (action, users) => {
  let newUsers = Array.copy(users);
  switch action {
  | Sort(RegistrationDate, Desc) => Array.sort(
      (a, b) => cmp(Js.Date.getTime(a.registration_date), Js.Date.getTime(b.registration_date)),
      newUsers
    )
  | Sort(RegistrationDate, Asc) => Array.sort(
    (a, b) => cmp(Js.Date.getTime(b.registration_date), Js.Date.getTime(a.registration_date)),
    newUsers
  )
  | Sort(Name, Desc) => Array.sort((a, b) => cmp(a.name, b.name), newUsers)
  | Sort(Name, Asc) => Array.sort((a, b) => cmp(b.name, a.name), newUsers)
  | Sort(Points, Desc) => Array.sort((a, b) => cmp(a.points_earned, b.points_earned), newUsers)
  | Sort(Points, Asc) => Array.sort((a, b) => cmp(b.points_earned, a.points_earned), newUsers)
  | _ => ()
  }
  newUsers
}

let component = ReasonReact.reducerComponent("ClientsList");

let make = (~users, ~maxVisibleCount=20, _children) => {
  ...component,
  initialState: () => {
    users: users,
    currentPage: 1,
    orderField: Name,
    orderBy: Asc,
  },
  didMount: self => self.send(Sort(self.state.orderField, self.state.orderBy)),
  willReceiveProps: self => {...self.state, users: sort(Sort(self.state.orderField, self.state.orderBy), users)},
  reducer: (action, state: state) => {
    switch action {
    | Sort(orderField, orderBy) =>
      ReasonReact.Update({users: sort(action, state.users), currentPage: 1, orderField, orderBy})
    | PageChange(page) =>
      ReasonReact.Update({...state, currentPage: page})
    }
  },
  render: ({state, send}) => {
    <>
      <table className="users_list_wrapper" width="100%">
      <thead>
        <tr className="users_list_head">
          <th className=("j-sortable" ++ switch (state.orderField, state.orderBy) {
            | (Name, Desc) => " j-sortable_active j-sortable_desc"
            | (Name, Asc) => " j-sortable_active j-sortable_asc"
            | _ => ""
            }) onClick=(switch (state.orderField, state.orderBy) {
            | (Name, Desc) => (_) => send(Sort(Name, Asc))
            | (Name, Asc) => (_) => send(Sort(Name, Desc))
            | _ => (_) => send(Sort(Name, Desc))
            })>
            <span>{ReasonReact.string({j|Имя и фамилия|j})}</span>
          </th>
          <th className=("j-sortable" ++ switch (state.orderField, state.orderBy) {
            | (RegistrationDate, Desc) => " j-sortable_active j-sortable_desc"
            | (RegistrationDate, Asc) => " j-sortable_active j-sortable_asc"
            | _ => ""
            }) onClick=(switch (state.orderField, state.orderBy) {
            | (RegistrationDate, Desc) => (_) => send(Sort(RegistrationDate, Asc))
            | (RegistrationDate, Asc) => (_) => send(Sort(RegistrationDate, Desc))
            | _ => (_) => send(Sort(RegistrationDate, Desc))
            })>
            <span>{ReasonReact.string({j|Дата регистрации|j})}</span>
          </th>
          <th className=("j-sortable" ++ switch (state.orderField, state.orderBy) {
            | (Points, Desc) => " j-sortable_active j-sortable_desc"
            | (Points, Asc) => " j-sortable_active j-sortable_asc"
            | _ => ""
            }) onClick=(switch (state.orderField, state.orderBy) {
            | (Points, Desc) => (_) => send(Sort(Points, Asc))
            | (Points, Asc) => (_) => send(Sort(Points, Desc))
            | _ => (_) => send(Sort(Points, Desc))
            })>
            <span>{ReasonReact.string({j|Баллы|j})}</span>
          </th>
        </tr>
      </thead>
      <tbody>
        (
          Array.sub(
            state.users,
            (state.currentPage - 1) * maxVisibleCount,
            min(Array.length(users) - ((state.currentPage - 1) * maxVisibleCount), maxVisibleCount)
          )
          |> Array.map(user => 
              <tr key=user.id className="users_list_body">
                <td>{ReasonReact.string(user.name)}</td>
                <td>
                  {ReasonReact.string(
                    MomentRe.momentWithDate(user.registration_date)
                    |> MomentRe.Moment.format("DD.MM.YYYY")
                  )}
                </td>
                <td>
                  <i className="user_points_icon" />
                  {ReasonReact.string(string_of_int(user.points_earned))}
                </td>
              </tr>
            )
          |> ReasonReact.array
        )
      </tbody>
      </table>

      <Pagination
        currentPage=state.currentPage
        totalCount={float_of_int(Array.length(users))}
        perPage={float_of_int(maxVisibleCount)}
        onPageClick=(num => send(PageChange(num))) />
    </>
  },
};
