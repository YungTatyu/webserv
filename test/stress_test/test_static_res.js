// import necessary modules
import { check } from "k6";
import http from "k6/http";

// define configuration
export const options = {
  // define thresholds
  // thresholds: {
  //   // http_req_failed: ["rate<0.01"], // http errors should be less than 1%
  //   http_req_duration: ["p(99)<1000"], // 99% of requests should be below 1s
  // },
};

export default function () {
  // define URL and request body
  const url = "http://localhost:4242/";

  // send a post request and save response as a variable
  const res = http.get(url);

  // check that response is 200
  check(res, {
    "response code was 200": (res) => res.status == 200,
  });
}
