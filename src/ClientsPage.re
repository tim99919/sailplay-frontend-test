[%raw {|require('./ClientsPage.less')|}]

type action =
  | Loading
  | Loaded(array(ClientsList.user), ClientsFilter.limits)
  | Error
  | UpdateFilter(ClientsFilter.filter);

type status =
  | DataLoading
  | DataLoaded
  | DataError;

type state = {
  status: status,
  users: option(array(ClientsList.user)),
  filteredUsers: option(array(ClientsList.user)),
  filter_limits: option(ClientsFilter.limits),
};

let contains = (pattern: string, target: string): bool => if (pattern == "") {
  true
} else {
  Js.Re.test(
    Js.String.toLowerCase(target),
    Js.Re.fromString(Js.String.toLowerCase(pattern))
  )
};

let filterUsers = (filter: ClientsFilter.filter, users) => switch users {
  | None => users
  | Some(users) => {
    let filtered = users
      |> Array.to_list
      |> List.filter((user: ClientsList.user) => if (
          contains(filter.name, user.name)
          && Js.Date.getTime(filter.registration_date_from) <= Js.Date.getTime(user.registration_date) && Js.Date.getTime(user.registration_date) <= Js.Date.getTime(filter.registration_date_to) 
          && filter.points_earned_from <= user.points_earned && user.points_earned <= filter.points_earned_to 
          && filter.points_spent_from <= user.points_spent && user.points_spent <= filter.points_spent_to
        ) {
          true
        } else {
          false
        })
    if (List.length(filtered) == 0) {
      None
    } else {
      Some(Array.of_list(filtered))
    }
  }
}

let component = ReasonReact.reducerComponent("ClientsPage");

let make = (_children) => {
  ...component,
  initialState: () => {
    status: DataLoading,
    users: None,
    filteredUsers: None,
    filter_limits: None,
  },
  reducer: (action, state) => {
    switch action {
    | Loaded(users, limits) =>
      ReasonReact.Update({
        status: DataLoaded,
        users: Some(users),
        filteredUsers: Some(users),
        filter_limits: Some(limits),
      })
    | Error => ReasonReact.Update({ ...state, status: DataError})
    | UpdateFilter(filter) => ReasonReact.Update({ ...state, filteredUsers: filterUsers(filter, state.users) })
    | _ => ReasonReact.NoUpdate
    }
  },
  didMount: self => {
    Js.Promise.(
      Data.fetchAll()
      |> then_(((limits, users)) => {
        self.send(Loaded(users, limits));
        resolve()
      })
      |> catch(_error => {
        self.send(Error);
        resolve()
      })
      |> ignore
    )
  },
  render: self => {
    let filteredUsersCount = switch self.state.filteredUsers {
    | None => 0
    | Some(users) => Array.length(users)
    };
    
    <div>
      <header className="route_title">
        <h1>
          {ReasonReact.string({j|Клиенты|j})}
        </h1>
      </header>

      <main>
        (
          switch self.state.status {
          | DataLoaded =>
            <main>
              (
                switch self.state.filter_limits {
                | None => ReasonReact.null
                | Some(limits) => <ClientsFilter
                    onSubmit=(filter => self.send(UpdateFilter(filter)))
                    filteredUsersCount=filteredUsersCount
                    limits=limits
                  />
                }
              )
              (
                switch self.state.filteredUsers {
                | None => ReasonReact.null
                | Some(users) => <ClientsList maxVisibleCount=10 users={users} />
                }
              )
            </main>
          | DataLoading => <div className="infobox">{ReasonReact.string({j|Загрузка данных…|j})}</div>
          | DataError => <div className="errorbox">{ReasonReact.string({j|Возникли ошибки при загрузке данных.|j})}</div>
          }
        )
      </main>
    </div>
  },
};
