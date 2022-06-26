import sys
import re

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as ec, wait

try:
    opts = webdriver.FirefoxOptions()
    opts.headless = True
    browser = webdriver.Firefox(options=opts)

    # opts = webdriver.ChromeOptions()
    # opts.headless = True
    # browser = webdriver.Chrome(options=opts)

    browser.get(sys.argv[1])

    wait = WebDriverWait(browser, 3)

    browser.execute_script("arguments[0].click();", wait.until(
        ec.presence_of_element_located((By.XPATH, "//button[contains(@class, 'btn-outline-info')]"))))

    for i in [i.get_attribute("src") for i in browser.find_elements(By.XPATH, "//img")]:
        if re.search(r'https://xfs-\d+\.mpcdn\.net/', i):
            print(i)

    # print(browser.page_source)
finally:
    try:
        browser.close()
    except:
        pass
