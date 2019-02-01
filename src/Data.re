module Decode = {
  let search_limits = json: ClientsFilter.limits =>
    Json.Decode.{
      min_registration_date: json |> field("min_registration_date", date),
      max_registration_date: json |> field("max_registration_date", date),
      min_points_earned: json |> field("min_points_earned", int),
      max_points_earned: json |> field("max_points_earned", int),
      min_points_spent: json |> field("min_points_spent", int),
      max_points_spent: json |> field("max_points_spent", int),
    };

  let user = json: ClientsList.user =>
    Json.Decode.{
      id: json |> field("id", string),
      name: json |> field("name", string),
      points_earned: json |> field("points_earned", int),
      points_spent: json |> field("points_spent", int),
      registration_date: json |> field("registration_date", date),
    };

  let users = json: array(ClientsList.user) =>
    Json.Decode.(
      json |> array(user)
    )
};

let fetchAll = () =>
  Js.Promise.(
    all2((
      Fetch.fetch("/assets/search_limits.json")
      |> then_(Fetch.Response.json)
      |> then_(json =>
        json
        |> Decode.search_limits
        |> resolve
      ),
      Fetch.fetch("/assets/users.json")
      |> then_(Fetch.Response.json)
      |> then_(json =>
        json
        |> Decode.users
        |> resolve
      )
    ))
  )