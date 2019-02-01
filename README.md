## [Demo](https://sailplay-filter-aaquqe0t1.now.sh/)

## Request params

* `name` (`?name=Adeline%20Ortiz`)
* `date_from`, `date_to` (`?date_from=2015-02-01&date_to=2018-02-01`)
* `earned_from`, `earned_to`
* `spent_from`, `spent_to`

## Notes

For local testing apply patch:

```diff
--- node_modules/react-input-range/lib/js/index.js
+++ node_modules/react-input-range/lib/js/index.js
@@ -54,5 +54,5 @@
  */

 exports.default = _inputRange2.default;
-module.exports = exports.default;
+module.exports = exports;
 //# sourceMappingURL=index.js.map
```

## Development

### Local

```sh
yarn install
yarn start
yarn run webpack
```

### Production build

```sh
yarn run build
yarn run webpack:production
```