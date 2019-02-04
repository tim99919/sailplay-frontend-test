[%raw {|require('react-input-range/lib/css/index.css')|}];
[%raw {|require('react-input-range/lib/css/input-range/input-range.css')|}];

[@bs.deriving abstract]
type values = {
  min: int,
  max: int,
};

let min = (o: values) => minGet(o);

let max = (o: values) => maxGet(o);

[@bs.deriving abstract]
type jsProps = {
  maxValue: int,
  minValue: int,
  step: int,
  value: values,
  onChange: values => unit,
};

[@bs.module "react-input-range/lib/js/"]
external reactClass: ReasonReact.reactClass = "default";

let make =
    (
      ~maxValue,
      ~minValue,
      ~value,
      ~step,
      ~onChange,
      children,
    ) =>
      ReasonReact.wrapJsForReason(
        ~reactClass,
        ~props=jsProps(
          ~maxValue,
          ~minValue,
          ~value,
          ~step,
          ~onChange,
        ),
        children,
      );